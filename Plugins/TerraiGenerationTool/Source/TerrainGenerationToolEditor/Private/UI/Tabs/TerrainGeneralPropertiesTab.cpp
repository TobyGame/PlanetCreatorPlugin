#include "TerrainGeneralPropertiesTab.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "Modules/ModuleManager.h"
#include "TerrainEditor.h"
#include "Assets/TerrainAsset.h"

FTerrainGeneralPropertiesTab::FTerrainGeneralPropertiesTab(TSharedPtr<FTerrainEditor> InEditor)
	: FTerrainTabFactory(FName("GeneralPropertiesTab"), InEditor)
{}

TSharedRef<SWidget> FTerrainGeneralPropertiesTab::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs Args;
	Args.bAllowSearch = true;
	Args.bHideSelectionTip = false;
	Args.bLockable = false;

	TSharedPtr<IDetailsView> DetailsView = PropertyEditorModule.CreateDetailView(Args);

	if (TerrainEditor.IsValid())
	{
		if (UObject* EditingObject = TerrainEditor.Pin()->GetEditingAsset())
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
