#include "Toolkit/TerrainEditor.h"
#include "Toolkit/TerrainEditorMode.h"
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

UTerrainGraph* FTerrainEditor::GetGraph() const
{
	if (TerrainGraph)
	{
		return TerrainGraph;
	}

	return nullptr;
}

#undef LOCTEXT_NAMESPACE