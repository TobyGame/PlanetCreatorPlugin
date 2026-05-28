#pragma once

#include "CoreMinimal.h"

#include "UTKPreviewTerrainTypes.generated.h"

class UMaterialInterface;
class UStaticMesh;

UENUM(BlueprintType)
enum class EUTKPreviewBackend : uint8
{
	None UMETA(DisplayName="None"),
	DynamicMesh UMETA(DisplayName="Dynamic Mesh"),
	HeightTexture UMETA(DisplayName="Nanite Height Texture"),
	ChunkedHeightTexture UMETA(DisplayName="Chunked Nanite Height Texture"),
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

	EUTKPreviewBackend PreferredBackend = EUTKPreviewBackend::DynamicMesh;

	TWeakObjectPtr<UStaticMesh> NanitePreviewMesh;
	TWeakObjectPtr<UMaterialInterface> NaniteDisplacementMaterial;

	static FUTKPreviewTerrainMapping Make(
		int32 InResolution,
		float InWidthMeters,
		float InMaxHeightMeters)
	{
		FUTKPreviewTerrainMapping Mapping;

		Mapping.Resolution = InResolution;
		Mapping.WidthMeters = InWidthMeters;
		Mapping.MaxHeightMeters = InMaxHeightMeters;
		Mapping.RefreshDerivedValues();

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

	FVector ToFlatPreviewPosition(float U, float V) const
	{
		return FVector(
			(U - 0.5f) * PreviewFootprintUU,
			(V - 0.5f) * PreviewFootprintUU,
			0.0f
		);
	}

	float GetPreviewHeightScaleUU() const
	{
		return PreviewFootprintUU * HeightScaleRatio;
	}
};