#pragma once

#include "CoreMinimal.h"


/**
 * Shared terrain data block passed between nodes.
 * Contains intermediate or final data such as heightmaps, masks, flow maps, etc.
 */

// Height map - Float array
struct FTerrainHeightmap
{
	TArray<float> Values;
	int32 Width = 0;
	int32 Height = 0;

	FORCEINLINE float Get(int32 X, int32 Y) const { return Values.IsValidIndex(Y * Width + X ) ? Values[Y * Width + X] : 0.f; }
	FORCEINLINE void Set(int32 X, int32 Y, float Value) { if (Values.IsValidIndex(Y * Width + X)) Values[Y * Width + X] = Value; }

	void Resize(int32 InWidth, int32 InHeight)
	{
		Width = InWidth;
		Height = InHeight;
		Values.SetNumZeroed(Width * Height);
	}
};

// Binary mask - uint8
struct FTerrainMask
{
	TArray<uint8> Values;
	int32 Width = 0;
	int32 Height = 0;

	FORCEINLINE uint8 Get(int32 X, int32 Y) const { return Values.IsValidIndex(Y * Width + X) ? Values[Y * Width + X] : 0; }
	FORCEINLINE void Set(int32 X, int32 Y, uint8 Value) { if (Values.IsValidIndex(Y * Width + X)) Values[Y * Width + X] = Value; }

	void Resize(int32 InWidth, int32 InHeight)
	{
		Width = InWidth;
		Height = InHeight;
		Values.SetNumZeroed(Width * Height);
	}
};

// Flow map - Vector 2D
struct FTerrainFlowMap
{
	TArray<FVector2D> Values;
	int32 Width = 0;
	int32 Height = 0;

	FORCEINLINE FVector2D Get(int32 X, int32 Y) const { return Values.IsValidIndex(Y * Width + X) ? Values[Y * Width + X] : FVector2D::ZeroVector; }
	FORCEINLINE void Set(int32 X, int32 Y, FVector2D Value) { if (Values.IsValidIndex(Y * Width + X)) Values[Y * Width + X] = Value; }

	void Resize(int32 InWidth, int32 InHeight)
	{
		Width = InWidth;
		Height = InHeight;
		Values.SetNumZeroed(Width * Height);
	}
};

// Debris map - float
struct FTerrainDebrisMap
{
	TArray<float> Values;
	int32 Width = 0;
	int32 Height = 0;

	FORCEINLINE float Get(int32 X, int32 Y) const { return Values.IsValidIndex(Y * Width + X) ? Values[Y * Width + X] : 0.f; }
	FORCEINLINE void Set(int32 X, int32 Y, float Value) { if (Values.IsValidIndex(Y * Width + X)) Values[Y * Width + X] = Value; }

	void Resize(int32 InWidth, int32 InHeight)
	{
		Width = InWidth;
		Height = InHeight;
		Values.SetNumZeroed(Width * Height);
	}
};

// Water map - float
struct FTerrainWaterMap
{
	TArray<float> Values;
	int32 Width = 0;
	int32 Height = 0;

	FORCEINLINE float Get(int32 X, int32 Y) const { return Values.IsValidIndex(Y * Width + X) ? Values[Y * Width + X] : 0.f; }
	FORCEINLINE void Set(int32 X, int32 Y, float Value) { if (Values.IsValidIndex(Y * Width + X)) Values[Y * Width + X] = Value; }

	void Resize(int32 InWidth, int32 InHeight)
	{
		Width = InWidth;
		Height = InHeight;
		Values.SetNumZeroed(Width * Height);
	}
};

// Main terrain data container
struct FTerrainData
{
	TSharedPtr<FTerrainHeightmap> Heightmap;
	TSharedPtr<FTerrainMask> Mask;
	TSharedPtr<FTerrainFlowMap> Flow;
	TSharedPtr<FTerrainDebrisMap> Debris;
	TSharedPtr<FTerrainWaterMap> Water;

	FName SourceNodeName;
	int32 Resolution = 512;

	/* Optional caching metadata */
	uint64 DataHash = 0;
	bool bIsTransient = true;
};