#include "TerrainGraphTab.h"

FTerrainGraphTab::FTerrainGraphTab(TSharedPtr<FTerrainEditor> InEditor)
	: FTerrainTabFactory(FName("GraphTab"), InEditor)
{}

TSharedRef<SWidget> FTerrainGraphTab::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return SNew(STextBlock)
		.Justification(ETextJustify::Center)
		.Text(FText::FromString("Graph Editor"));
}