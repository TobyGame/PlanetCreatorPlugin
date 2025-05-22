#include "Toolkit/TerrainEditor.h"

#include "GraphEditAction.h"
#include "Toolkit/TerrainEditorMode.h"
#include "Modules/ModuleManager.h"
#include "Assets/TerrainAsset.h"
#include "Graph/TerrainGraph.h"
#include "Graph/TerrainGraphBuilder.h"
#include "Graph/TerrainGraphSchema.h"

#define LOCTEXT_NAMESPACE "TerrainEditor"

void FTerrainEditor::InitTerrainEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* TerrainAsset)
{
	EditingObject = Cast<UTerrainAsset>(TerrainAsset);

	if (!EditingObject)
		return;

	if (!EditingObject->SavedGraph)
	{
		EditingObject->SavedGraph = NewObject<UTerrainGraphSaveData>(EditingObject, TEXT("SavedGraph"), RF_Transactional);
	}

	TerrainGraph = NewObject<UTerrainGraph>(GetTransientPackage(), TEXT("TerrainGraph"), RF_Transactional);
	TerrainGraph->Schema = UTerrainGraphSchema::StaticClass();

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

	FTerrainGraphBuilder::LoadFromAsset(EditingObject, TerrainGraph);

	GraphChangedListenerHandle = TerrainGraph->AddOnGraphChangedHandler(
		FOnGraphChanged::FDelegate::CreateSP(this, &FTerrainEditor::OnGraphChanged)
	);
}

UTerrainGraph* FTerrainEditor::GetGraph() const
{
	if (TerrainGraph)
	{
		return TerrainGraph;
	}

	return nullptr;
}

void FTerrainEditor::OnClose()
{
	FTerrainGraphBuilder::SaveToAsset(TerrainGraph, EditingObject);

	if (TerrainGraph)
		TerrainGraph->RemoveOnGraphChangedHandler(GraphChangedListenerHandle);

	FWorkflowCentricApplication::OnClose();
}

void FTerrainEditor::OnGraphChanged(const FEdGraphEditAction& Action)
{
	if (!TerrainGraph || !EditingObject)
		return;

	FTerrainGraphBuilder::SaveToAsset(TerrainGraph, EditingObject);
}

#undef LOCTEXT_NAMESPACE