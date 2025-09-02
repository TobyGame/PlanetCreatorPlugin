#include "UI/Tabs/UTKGeneralPropertiesTab.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "Modules/ModuleManager.h"
#include "UI/Toolkit/UTKEditorApp.h"
#include "Assets/UTKAsset.h"

FUTKGeneralPropertiesTab::FUTKGeneralPropertiesTab(TSharedPtr<FUTKEditorApp> InEditor)
	: FUTKTabFactory(FName("GeneralPropertiesTab"), InEditor), Editor(InEditor)
{
	TabLabel = FText::FromString("General Properties");
	ViewMenuDescription = TabLabel;
}

TSharedRef<SWidget> FUTKGeneralPropertiesTab::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs Args;
	Args.bAllowSearch = true;
	Args.bHideSelectionTip = false;
	Args.bLockable = false;

	TSharedPtr<IDetailsView> DetailsView = PropertyEditorModule.CreateDetailView(Args);

	if (UTKEditor.IsValid())
	{
		if (UObject* EditingObject = UTKEditor.Pin()->GetEditingAsset())
		{
			DetailsView->SetObject(EditingObject);
		}
	}

	return SNew(SBox)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			DetailsView.ToSharedRef()
		];
}