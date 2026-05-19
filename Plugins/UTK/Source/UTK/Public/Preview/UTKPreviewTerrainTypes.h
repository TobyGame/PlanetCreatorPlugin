#pragma once

#include "CoreMinimal.h"

enum class EUTKPreviewBackend : uint8
{
	None,
	DynamicMesh,
	HeightTexture,
	ChunkedHeightTexture,
};

struct FUTKPreviewTerrainMapping
{
	int32 Resolution = 512;

	float WidthMeters = 5000.0f;
	float MaxHeightMeters = 2500.0f;

	float MetersPerPixel = 0.0f;
	float HeightScaleRatio = 0.5f;

	// Fixed viewport footprint.
	// The terrain preview will occupy this size in editor units regardless of WidthMeters.
	float PreviewFootprintUU = 100.0f;

	static FUTKPreviewTerrainMapping Make(
		int32 InResolution,
		float InWidthMeters,
		float InMaxHeightMeters)
	{
		FUTKPreviewTerrainMapping Mapping;

		Mapping.Resolution = InResolution;
		Mapping.WidthMeters = InWidthMeters;
		Mapping.MaxHeightMeters = InMaxHeightMeters;



		return Mapping;
	}

	void RefreshDerivedValues()
	{
		MetersPerPixel = Resolution > 0
			? WidthMeters / static_cast<float>(Resolution)
			: 0.0f;

		HeightScaleRatio = WidthMeters > 0.0f
			? MaxHeightMeters / WidthMeters
			: 0.0f;
	}

	FVector ToPreviewPosition(float U, float V, float Height01) const
	{
		return FVector(
			(U - 0.5f) * PreviewFootprintUU,
			(V - 0.5f) * PreviewFootprintUU,
			Height01 * PreviewFootprintUU * HeightScaleRatio
		);
	}
};