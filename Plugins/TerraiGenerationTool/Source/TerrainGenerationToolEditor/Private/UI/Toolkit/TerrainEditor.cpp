#include "Toolkit/TerrainEditor.h"
#include "Toolkit/TerrainEditorMode.h"
#include "Modules/ModuleManager.h"
#include "Assets/TerrainAsset.h"
#include "Framework/Commands/GenericCommands.h"
#include "Graph/TerrainGraph.h"
#include "Graph/TerrainGraphBuilder.h"
#include "Graph/TerrainGraphSchema.h"
#include "Graph/Nodes/TerrainNode.h"

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

	//CommandList = MakeShareable(new FUICommandList);
	GetToolkitCommands()->MapAction(
		FGenericCommands::Get().Delete,
		FExecuteAction::CreateSP(this, &FTerrainEditor::DeleteSelectedNodes),
		FCanExecuteAction::CreateSP(this, &FTerrainEditor::CanDeleteNodes)
	);

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

void FTerrainEditor::DeleteSelectedNodes()
{
	if (!GraphUI.IsValid())
		return;

	const FGraphPanelSelectionSet SelectedNodes = GraphUI->GetSelectedNodes();
	if (SelectedNodes.Num() == 0)
		return;

	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEditor_DeleteNode", "Delete Nodes"));
	TerrainGraph->Modify();

	for (UObject* NodeObj : SelectedNodes)
	{
		if (UTerrainNode* Node = Cast<UTerrainNode>(NodeObj))
		{
			Node->Modify();
			Node->GetGraph()->RemoveNode(Node);
		}
	}

	GraphUI->ClearSelectionSet();
}

bool FTerrainEditor::CanDeleteNodes()
{
	return GraphUI.IsValid()
		&& GraphUI->GetSelectedNodes().Num() > 0
		&& FSlateApplication::Get().GetKeyboardFocusedWidget() == GraphUI;
}

#undef LOCTEXT_NAMESPACE