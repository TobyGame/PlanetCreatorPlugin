#include "Preview/UTKTerrainPreviewComponent.h"

UUTKTerrainPreviewComponent::UUTKTerrainPreviewComponent()
{
	SetMobility(EComponentMobility::Movable);
	SetCastShadow(false);
	SetVisibility(false);
	bSelectable = false;
}

void UUTKTerrainPreviewComponent::ClearPreview()
{
	SetStaticMesh(nullptr);
	SetVisibility(false);

	bHasValidPreview = false;
	CurrentLayerName = NAME_None;
	CurrentMapping = FUTKPreviewTerrainMapping();
}

void UUTKTerrainPreviewComponent::UpdateFromTerrain(
	const FUTKTerrain& Terrain,
	FName LayerName,
	const FUTKPreviewTerrainMapping& Mapping)
{
	CurrentLayerName = LayerName;
	CurrentMapping = Mapping;
	bHasValidPreview = true;

	SetVisibility(false);
}