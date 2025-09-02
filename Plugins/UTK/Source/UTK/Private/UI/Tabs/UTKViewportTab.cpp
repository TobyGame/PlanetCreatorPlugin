#include "UI/Tabs/UTKViewportTab.h"

FUTKViewportTab::FUTKViewportTab(TSharedPtr<FUTKEditorApp> InEditor)
	: FUTKTabFactory(FName("ViewportTab"), InEditor), Editor(InEditor)
{}

TSharedRef<SWidget> FUTKViewportTab::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return SNew(STextBlock)
		.Justification(ETextJustify::Center)
		.Text(FText::FromString("Viewport"));
}