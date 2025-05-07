#include "Toolkit/TerrainTabFactory.h"
#include "Toolkit/TerrainEditor.h"

FTerrainTabFactory::FTerrainTabFactory(FName InTabID, TSharedPtr<FTerrainEditor> InEditor)
	: FWorkflowTabFactory(InTabID, InEditor), TerrainEditor(InEditor)
{
	TabLabel = FText::FromString(InTabID.ToString());
	bIsSingleton = true;
}