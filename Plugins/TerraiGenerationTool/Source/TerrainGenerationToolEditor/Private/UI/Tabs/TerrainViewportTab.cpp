#include "TerrainViewportTab.h"

FTerrainViewportTab::FTerrainViewportTab(TSharedPtr<FTerrainEditor> InEditor)
	: FTerrainTabFactory(FName("ViewportTab"), InEditor), Editor(InEditor)
{}

TSharedRef<SWidget> FTerrainViewportTab::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return SNew(STextBlock)
		.Justification(ETextJustify::Center)
		.Text(FText::FromString("Viewport"));
}