#include "TerrainEditor.h"
#include "TerrainEditor.h"
#include "TerrainEditorMode.h"
#include "Modules/ModuleManager.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "TerrainEditor"

void FTerrainEditor::InitTerrainEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* TerrainAsset)
{
	EditingObject = TerrainAsset;

	TerrainEditorMode = MakeShareable(new FTerrainEditorMode(SharedThis(this)));

	InitAssetEditor(Mode, InitToolkitHost, FName("TerrainEditorApp"), TerrainEditorMode->GetLayout(), true, true, TerrainAsset);
}

void FTerrainEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager)
{
	FWorkflowCentricApplication::RegisterTabSpawners(TabManager);
}

void FTerrainEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManager)
{
	FWorkflowCentricApplication::UnregisterTabSpawners(TabManager);
}

#undef LOCTEXT_NAMESPACE