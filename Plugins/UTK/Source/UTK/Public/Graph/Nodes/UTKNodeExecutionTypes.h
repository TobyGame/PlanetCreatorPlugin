#pragma once

#include "CoreMinimal.h"
#include "Core/UTKTerrainTypes.h"

class UUTKNode;

/**
 * Execution context passed to every node Process() call.
 * For now: resolution and seed.
 */
struct UTK_API FUTKNodeExecutionContext
{
	int32 ResolutionX = 0;
	int32 ResolutionY = 0;
	int32 Seed = 0;

	FUTKNodeExecutionContext() = default;

	FUTKNodeExecutionContext(int32 InResolutionX, int32 InResolutionY, int32 InSeed)
		: ResolutionX(InResolutionX), ResolutionY(InResolutionY), Seed(InSeed)
	{}

	FORCEINLINE bool IsValid() const
	{
		return ResolutionX > 0 && ResolutionY > 0;
	}
};

/**
 * One input for a node: a pointer to a terrain package plus
 * a "default layer" name that the node should treat as the main channel
 * (usually "Height", but can be "Mask", "Sediment", etc.).
 */
struct UTK_API FUTKNodeInput
{
	/** Terrain package produced by an upstream node. May be null if input is unconnected. */
	TSharedPtr<FUTKTerrain> Terrain;

	/** Name of the main layer to use from this terrain (e.g., "Height", "Mask"). */
	FName DefaultLayerName = NAME_None;

	FUTKNodeInput() = default;

	FUTKNodeInput(const TSharedPtr<FUTKTerrain>& InTerrain, FName InDefaultLayerName = NAME_None)
		: Terrain(InTerrain), DefaultLayerName(InDefaultLayerName)
	{}

	/** Returns true if this input has a valid terrain package. */
	FORCEINLINE bool HasTerrain() const
	{
		return Terrain.IsValid() && Terrain->IsValid();
	}

	/** Try to get the default layer as const. Return nullptr if missing. */
	const FUTKLayer* FindDefaultLayer() const
	{
		if (!HasTerrain() || DefaultLayerName.IsNone())
			return nullptr;

		return Terrain->FindLayer(DefaultLayerName);
	}

	/** Try to get the default layer as mutable. Returns nullptr if missing. */
	FUTKLayer* FindDefaultLayer()
	{
		if (!HasTerrain() || DefaultLayerName.IsNone())
			return nullptr;

		return Terrain->FindLayer(DefaultLayerName);
	}
};

/**
 * One output of a node: a terrain package produced by the node plus
 * a "default layer" name that the corresponding output pin exposes
 * (e.g., "Height", "Sediment", "Flow", etc.).
 */
struct UTK_API FUTKNodeOutput
{
	/** Terrain package produced by this node. */
	TSharedPtr<FUTKTerrain> Terrain;

	/** Name of the main layer for this output pin */
	FName DefaultLayerName = NAME_None;

	FUTKNodeOutput() = default;

	FUTKNodeOutput(const TSharedPtr<FUTKTerrain>& InTerrain, FName InDefaultLayerName = NAME_None)
		: Terrain(InTerrain), DefaultLayerName(InDefaultLayerName)
	{}

	FORCEINLINE bool HasTerrain() const
	{
		return Terrain.IsValid() && Terrain->IsValid();
	}

	/** Try to get the default layer as const. Return nullptr if missing. */
	const FUTKLayer* FindDefaultLayer() const
	{
		if (!HasTerrain() || DefaultLayerName.IsNone())
			return nullptr;

		return Terrain->FindLayer(DefaultLayerName);
	}

	/** Try to get the default layer as mutable. Returns nullptr if missing. */
	FUTKLayer* FindDefaultLayer()
	{
		if (!HasTerrain() || DefaultLayerName.IsNone())
			return nullptr;

		return Terrain->FindLayer(DefaultLayerName);
	}
};


/**
 * Standard node processing function type.
 * 
 * - Inputs:	one entry per input pin.
 * - Outputs:	one entry per output pin (the function must fill this).
 * - Ctx:		execution context (resolution, seed, etc.).
 * - Workspace: node-local scratch buffers, not exposed as outputs.
 * 
 * Node will implement this via macro-based system.
 */
using FUTKNodeProcessFunction = TFunction<void(
	const TArray<FUTKNodeInput>& Inputs,
	TArray<FUTKNodeOutput>& Outputs,
	FUTKNodeExecutionContext& Ctx,
	FUTKTerrainWorkspace& Workspace,
	UUTKNode& Node
)>;