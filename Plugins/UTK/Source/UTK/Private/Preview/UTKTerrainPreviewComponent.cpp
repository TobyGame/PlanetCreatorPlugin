#include "Preview/UTKTerrainPreviewComponent.h"

UUTKTerrainPreviewComponent::UUTKTerrainPreviewComponent()
{
	SetMobility(EComponentMobility::Movable);
	SetVisibility(false, true);
}

void UUTKTerrainPreviewComponent::ClearPreview()
{
	SetStaticMesh(nullptr);
	SetVisibility(false);

	bHasValidPreview = false;
	CurrentLayerName = NAME_None;
	CurrentMapping = FUTKPreviewTerrainMapping();
	ActiveBackendType = EUTKPreviewBackend::None;
}

void UUTKTerrainPreviewComponent::UpdateFromTerrain(
	const FUTKTerrain& Terrain,
	FName LayerName,
	const FUTKPreviewTerrainMapping& Mapping)
{
	CurrentLayerName = LayerName;
	CurrentMapping = Mapping;
	CurrentMapping.RefreshDerivedValues();

	bHasValidPreview = true;

	ActiveBackendType = EUTKPreviewBackend::None;
	SetVisibility(false, true);
}

FBoxSphereBounds UUTKTerrainPreviewComponent::GetPreviewBounds() const
{
	if (!bHasValidPreview)
		return FBoxSphereBounds(GetComponentLocation(), FVector::ZeroVector, 0.0f);

	const float HalfFootprintUU = FMath::Max(0.0f, CurrentMapping.PreviewFootprintUU) * 0.5f;
	const float PreviewHeightUU = FMath::Max(0.0f, CurrentMapping.PreviewFootprintUU * CurrentMapping.HeightScaleRatio);

	const FVector LocalOrigin(0.0f, 0.0f, PreviewHeightUU * 0.5f);
	const FVector LocalExtent(HalfFootprintUU, HalfFootprintUU, PreviewHeightUU * 0.5f);

	return FBoxSphereBounds(LocalOrigin, LocalExtent, LocalExtent.Size()).TransformBy(GetComponentTransform());
}