#pragma once

#include "CoreMinimal.h"


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

		Mapping.MetersPerPixel = InResolution > 0
			? InWidthMeters / static_cast<float>(InResolution)
			: 0.0f;

		Mapping.HeightScaleRatio = InMaxHeightMeters > 0.0f
			? InWidthMeters / InMaxHeightMeters
			: 0.0f;

		return Mapping;
	}
};