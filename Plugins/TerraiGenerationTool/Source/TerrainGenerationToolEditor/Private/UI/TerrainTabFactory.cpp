#include "TerrainTabFactory.h"
#include "TerrainEditor.h"

FTerrainTabFactory::FTerrainTabFactory(FName InTabID, TSharedPtr<FTerrainEditor> InEditor)
	: FWorkflowTabFactory(InTabID, InEditor), TerrainEditor(InEditor)
{
	TabLabel = FText::FromString(InTabID.ToString());
	bIsSingleton = true;
}