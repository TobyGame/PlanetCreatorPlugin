#include "TerrainEditor.h"
#include "TerrainEditorMode.h"
#include "Modules/ModuleManager.h"
#include "Assets/TerrainAsset.h"

#define LOCTEXT_NAMESPACE "TerrainEditor"

void FTerrainEditor::InitTerrainEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* TerrainAsset)
{
	EditingObject = Cast<UTerrainAsset>(TerrainAsset);

	InitAssetEditor(
		Mode,
		InitToolkitHost,
		FName("TerrainEditorApp"),
		FTabManager::FLayout::NullLayout,
		true,
		true,
		TerrainAsset
	);

	AddApplicationMode(TEXT("TerrainEditorMode"), MakeShareable(new FTerrainEditorMode(SharedThis(this))));

	SetCurrentMode(TEXT("TerrainEditorMode"));
}

void FTerrainEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FWorkflowCentricApplication::RegisterTabSpawners(InTabManager);
}

void FTerrainEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FWorkflowCentricApplication::UnregisterTabSpawners(InTabManager);
}

#undef LOCTEXT_NAMESPACE