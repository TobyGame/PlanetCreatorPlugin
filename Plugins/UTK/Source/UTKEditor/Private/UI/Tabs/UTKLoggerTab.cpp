#include "UTKLoggerTab.h"

FUTKLoggerTab::FUTKLoggerTab(TSharedPtr<FUTKEditorApp> InEditor)
	: FUTKTabFactory(FName("LoggerTab"), InEditor), Editor(InEditor)
{}

TSharedRef<SWidget> FUTKLoggerTab::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return SNew(STextBlock)
		.Justification(ETextJustify::Center)
		.Text(FText::FromString("Debug Logger - Output Window"));
}