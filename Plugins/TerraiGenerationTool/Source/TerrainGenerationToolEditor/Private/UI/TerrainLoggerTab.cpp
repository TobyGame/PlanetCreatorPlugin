#include "TerrainLoggerTab.h"

FTerrainLoggerTab::FTerrainLoggerTab(TSharedPtr<FTerrainEditor> InEditor)
	: FTerrainTabFactory(FName("LoggerTab"), InEditor)
{}

TSharedRef<SWidget> FTerrainLoggerTab::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return SNew(STextBlock)
		.Justification(ETextJustify::Center)
		.Text(FText::FromString("Debug Logger - Output Window"));
}
