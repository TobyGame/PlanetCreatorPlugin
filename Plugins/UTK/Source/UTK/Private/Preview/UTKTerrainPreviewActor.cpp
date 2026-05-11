#include "Preview/UTKTerrainPreviewActor.h"

#include "Preview/UTKTerrainPreviewComponent.h"

AUTKTerrainPreviewActor::AUTKTerrainPreviewActor()
{
	PrimaryActorTick.bCanEverTick = false;

	PreviewComponent = CreateDefaultSubobject<UUTKTerrainPreviewComponent>(TEXT("TerrainPreviewComponent"));
	SetRootComponent(PreviewComponent);

	SetActorHiddenInGame(false);
}