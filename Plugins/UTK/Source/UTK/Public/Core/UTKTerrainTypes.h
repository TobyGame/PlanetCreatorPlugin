#pragma once

#include "CoreMinimal.h"

/**
 * Simple 2D domain descriptor used by all terrain buffers and packages.
 * Later we can extend this with world-space extents if needed.
 */
struct UTK_API FUTKDomain2D
{
	int32 Width = 0;
	int32 Height = 0;

	FUTKDomain2D() = default;

	FUTKDomain2D(int32 InWidth, int32 InHeight)
		: Width(InWidth), Height(InHeight)
	{}

	FORCEINLINE bool IsValid() const
	{
		return Width > 0 && Height > 0;
	}

	FORCEINLINE int32 Num() const
	{
		return Width * Height;
	}
};

struct UTK_API FUTKBuffer2D
{
	TArray<float> Values;
	int32 Width = 0;
	int32 Height = 0;

	FUTKBuffer2D() = default;

	FUTKBuffer2D(int32 InWidth, int32 InHeight)
	{
		Initialize(InWidth, InHeight);
	}


	/**
	 * Allocate / resize the buffer to the given resolution.
	 * Does not initialize values (call Fill() for that).
	 */
	void Initialize(int32 InWidth, int32 InHeight)
	{
		Width = InWidth;
		Height = InHeight;

		const int32 NumValues = FMath::Max(Width * Height, 0);
		Values.SetNumUninitialized(NumValues);
	}

	FORCEINLINE bool IsValid() const
	{
		return Width > 0 && Height > 0 && Values.Num() == Width * Height;
	}

	/** Row-major index helper. No bound checks. */
	FORCEINLINE int32 IndexUnchecked(int32 X, int32 Y) const
	{
		return Y * Width + X;
	}

	/** Safe index helper with check(). */
	FORCEINLINE int32 IndexChecked(int32 X, int32 Y) const
	{
		check(Width > 0 && Height > 0);
		check(X >= 0 && X < Height);
		check(Y >= 0 && Y < Width);
		return Y * Width + X;
	}

	FORCEINLINE float Get(int32 X, int32 Y) const
	{
		return Values[IndexChecked(X, Y)];
	}

	FORCEINLINE void Set(int32 X, int32 Y, float V)
	{
		Values[IndexChecked(X, Y)] = V;
	}

	/** Fill the entire buffer with a constant value. */
	void Fill(float V)
	{
		for (float& Value : Values)
			Value = V;
	}
};

/**
 * One named layer inside a terrain package.
 * Example Names: "Height", "Sediment", "Flow", "Debris", "Moisture", etc.
 * 
 * The Data reference is shared so multiple views / branches can reference
 * the same underlying buffer (copy-on-write can be added later).
 */
struct UTK_API FUTKLayer
{
	/** Logical name of the layer inside a terrain package. */
	FName Name;

	/** Shared buffer backing this layer. */
	TSharedRef<FUTKBuffer2D> Data;

	FUTKLayer(const FName& InName, const TSharedRef<FUTKBuffer2D>& InData)
		: Name(InName), Data(InData)
	{}
};

/**
 * A terrain "package" flowing between nodes.
 * It represents a 2D domain and an arbitrary set of named layers.
 * 
 * Examples:
 * - Height-only package: Layers["Height"]
 * - Erosion result: Layers["Height"], Layers["Sediment"], Layers["Flow"], Layers["Debris"]
 */
struct UTK_API FUTKTerrain
{
	/** Grid domain shared by all layer. */
	FUTKDomain2D Domain;

	/** Name -> Layer map. */
	TMap<FName, FUTKLayer> Layers;

	FUTKTerrain() = default;

	explicit FUTKTerrain(const FUTKDomain2D& InDomain)
		: Domain(InDomain)
	{}

	FORCEINLINE bool IsValid() const
	{
		return Domain.IsValid();
	}

	/** Returns const layer pointer if present, otherwise nullptr. */
	const FUTKLayer* FindLayer(const FName& Name) const
	{
		return Layers.Find(Name);
	}

	/** Returns mutable layer pointer if present, otherwise nullptr. */
	FUTKLayer* FindLayer(FName Name)
	{
		return Layers.Find(Name);
	}

	/**
	 * Returns any layer contained in this terrain, or nullptr if there are none.
	 * This is useful for generic nodes that don't care about specific names.
	 */
	const FUTKLayer* FindAnyLayer() const
	{
		if (Layers.Num() == 0)
			return nullptr;

		for (const TPair<FName, FUTKLayer>& Pair : Layers)
			return &Pair.Value;

		return nullptr;
	}

	/**
 	* Returns any layer contained in this terrain, or nullptr if there are none.
 	* This is useful for generic nodes that don't care about specific names.
 	*/
	FUTKLayer* FindAnyLayer()
	{
		if (Layers.Num() == 0)
			return nullptr;

		for (TPair<FName, FUTKLayer>& Pair : Layers)
			return &Pair.Value;

		return nullptr;
	}

	/** 
	 * Find or create a layer with the package domain.
	 * If created, allocates and sizes the buffer Domain.
	 */
	FUTKLayer& GetOrCreateLayer(FName Name)
	{
		check(Domain.IsValid());

		if (FUTKLayer* Existing = Layers.Find(Name))
		{
			// Ensure buffe matches domain (if Domain changed)
			if (!Existing->Data->IsValid() ||
				Existing->Data->Width != Domain.Width ||
				Existing->Data->Height != Domain.Height)
			{
				Existing->Data->Initialize(Domain.Width, Domain.Height);
			}

			return *Existing;
		}

		TSharedRef<FUTKBuffer2D> NewData = MakeShared<FUTKBuffer2D>(Domain.Width, Domain.Height);
		FUTKLayer NewLayer(Name, NewData);
		return Layers.Add(Name, MoveTemp(NewLayer));
	}
};

/**
 * Per-node, per-evaluation transient workspace.
 * 
 * Used for internal scratch data during node processing
 * (e.g., temporary water depth, capacity field, intermediate height buffers, etc.).
 * 
 * Workspace data is never exposed as graph outputs and is not cached.
 */
struct UTK_API FUTKTerrainWorkspace
{
	/** Domain this workspace operates on (usually sam as the node's output domain). */
	FUTKDomain2D Domain;

	/** Named scratch layers used internally by a node implementation. */
	TMap<FName, TSharedRef<FUTKBuffer2D>> ScratchLayers;

	FUTKTerrainWorkspace() = default;

	explicit FUTKTerrainWorkspace(const FUTKDomain2D& InDomain)
		: Domain(InDomain)
	{}

	/** 
	 * Clear all scratch buffers.
	 * Called typically at the start or end of a node evaluation
	 */
	void Clear()
	{
		ScratchLayers.Reset();
	}

	/**
	 * Find or create a scratch buffer sized to Domain.
	 * Use for transient intermediates (water depth, temp height, etc.).
	 */
	TSharedRef<FUTKBuffer2D> GetOrCreateScratch(FName Name)
	{
		check(Domain.IsValid());

		if (TSharedRef<FUTKBuffer2D>* Existing = ScratchLayers.Find(Name))
		{
			TSharedRef<FUTKBuffer2D> Ref = *Existing;
			if (!Ref->IsValid() ||
				Ref->Width != Domain.Width ||
				Ref->Height != Domain.Height)
			{
				Ref->Initialize(Domain.Width, Domain.Height);
			}
			return Ref;
		}

		TSharedRef<FUTKBuffer2D> NewData = MakeShared<FUTKBuffer2D>(Domain.Width, Domain.Height);
		ScratchLayers.Add(Name, NewData);
		return NewData;
	}
};