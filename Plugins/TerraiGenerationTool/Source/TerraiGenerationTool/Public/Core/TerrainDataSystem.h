#pragma once

#include "CoreMinimal.h"

/**
 * A basic 2D floating-point buffer used by terrain nodes.
 */
struct FTerrain2D
{
	TArray<float> Values;
	int32 Width = 0;
	int32 Height = 0;

	FTerrain2D() = default;

	FTerrain2D(int32 InWidth, int32 InHeight)
	{
		Resize(InWidth, InHeight);
	}

	void Resize(int32 InWidth, int32 InHeight)
	{
		Width = InWidth;
		Height = InHeight;
		Values.SetNumZeroed(Width * Height);
	}

	FORCEINLINE float GEt(int32 X, int32 Y)
	{
		check(X >= 0 && X < Width && Y >= 0 && Y < Height);
		return Values[Y * Width + X];
	}

	FORCEINLINE void Set(int32 X, int32 Y, float Value)
	{
		check(X >= 0 && X < Width && Y >= 0 && Y < Height);
		Values[Y * Width + X] = Value;
	}

	void Zero()
	{
		for (float& V : Values)
		{
			V = 0.0f;
		}
	}
};


/**
 * A dynamic data registry for terrain processing nodes.
 */
class FTerrainDataSet
{
public:
	TSharedPtr<FTerrain2D> Get(const FName& Name) const
	{
		const TSharedPtr<FTerrain2D>* Found = DataMap.Find(Name);
		return Found ? *Found : nullptr;
	}

	TSharedPtr<FTerrain2D> GetOrCreate(const FName& Name, int32 Width, int32 Height)
	{
		if (!DataMap.Contains(Name))
		{
			DataMap.Add(Name, MakeShared<FTerrain2D>(Width, Height));
		}
		return DataMap[Name];
	}

	void Set(const FName& Name, TSharedPtr<FTerrain2D> Data)
	{
		DataMap.Add(Name, Data);
	}

	void Invalidate(const FName& Name)
	{
		DataMap.Remove(Name);
	}

	void Clear()
	{
		DataMap.Reset();
	}

private:
	TMap<FName, TSharedPtr<FTerrain2D>> DataMap;
};