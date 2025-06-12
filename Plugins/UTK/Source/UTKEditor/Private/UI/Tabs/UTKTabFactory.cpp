#include "Tabs//UTKTabFactory.h"
#include "Toolkit/UTKEditorApp.h"

FUTKTabFactory::FUTKTabFactory(FName InTabID, TSharedPtr<FUTKEditorApp> InEditor)
	: FWorkflowTabFactory(InTabID, InEditor), UTKEditor(InEditor)
{
	TabLabel = FText::FromString(InTabID.ToString());
	bIsSingleton = true;
}