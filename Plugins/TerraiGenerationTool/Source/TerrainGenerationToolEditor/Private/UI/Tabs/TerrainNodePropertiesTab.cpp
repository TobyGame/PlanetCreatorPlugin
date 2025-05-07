#include "TerrainNodePropertiesTab.h"

FTerrainNodePropertiesTab::FTerrainNodePropertiesTab(TSharedPtr<FTerrainEditor> InEditor)
	: FTerrainTabFactory(FName("NodePropertiesTab"), InEditor), Editor(InEditor)
{}

TSharedRef<SWidget> FTerrainNodePropertiesTab::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return SNew(STextBlock)
		.Justification(ETextJustify::Center)
		.Text(FText::FromString("Node Properties"));
}