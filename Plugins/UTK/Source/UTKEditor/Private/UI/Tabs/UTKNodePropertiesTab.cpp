#include "UTKNodePropertiesTab.h"

FUTKNodePropertiesTab::FUTKNodePropertiesTab(TSharedPtr<FUTKEditorApp> InEditor)
	: FUTKTabFactory(FName("NodePropertiesTab"), InEditor), Editor(InEditor)
{}

TSharedRef<SWidget> FUTKNodePropertiesTab::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return SNew(STextBlock)
		.Justification(ETextJustify::Center)
		.Text(FText::FromString("Node Properties"));
}