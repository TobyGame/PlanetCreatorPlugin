#include "UI/Toolkit/UTKEditorApp.h"
#include "UI/Toolkit/UTKEditorMode.h"
#include "Modules/ModuleManager.h"
#include "Assets/UTKAsset.h"
#include "Framework/Commands/GenericCommands.h"
#include "Graph/UTKGraph.h"
#include "Graph/UTKGraphBuilder.h"
#include "Graph/UTKGraphSchema.h"
#include "Graph/Nodes/UTKNode.h"

#define LOCTEXT_NAMESPACE "UTKEditor"

void FUTKEditorApp::InitUTKEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* UTKAsset)
{
	EditingObject = Cast<UUTKAsset>(UTKAsset);

	if (!EditingObject)
		return;

	if (!EditingObject->SavedGraph)
	{
		EditingObject->SavedGraph = NewObject<UUTKGraphSaveData>(EditingObject, TEXT("SavedGraph"), RF_Transactional);
	}

	GEditor->RegisterForUndo(this);

	// Commands list for the editor
	GetToolkitCommands()->MapAction(
		FGenericCommands::Get().Delete,
		FExecuteAction::CreateSP(this, &FUTKEditorApp::DeleteSelectedNodes),
		FCanExecuteAction::CreateSP(this, &FUTKEditorApp::CanDeleteNodes)
	);

	GetToolkitCommands()->MapAction(
		FGenericCommands::Get().Undo,
		FExecuteAction::CreateSP(this, &FUTKEditorApp::Undo)
	);

	GetToolkitCommands()->MapAction(
		FGenericCommands::Get().Redo,
		FExecuteAction::CreateSP(this, &FUTKEditorApp::Redo)
	);

	UTKGraph = NewObject<UUTKGraph>(GetTransientPackage(), TEXT("UTKGraph"), RF_Transactional);
	UTKGraph->Schema = UUTKGraphSchema::StaticClass();

	InitAssetEditor(
		Mode,
		InitToolkitHost,
		FName("UTKEditorApp"),
		FTabManager::FLayout::NullLayout,
		true,
		true,
		UTKAsset
	);

	AddApplicationMode(TEXT("UTKEditorMode"), MakeShareable(new FUTKEditorMode(SharedThis(this))));
	SetCurrentMode(TEXT("UTKEditorMode"));

	FUTKGraphBuilder::LoadFromAsset(EditingObject, UTKGraph);

	GraphChangedListenerHandle = UTKGraph->AddOnGraphChangedHandler(
		FOnGraphChanged::FDelegate::CreateSP(this, &FUTKEditorApp::OnGraphChanged)
	);
}

UUTKGraph* FUTKEditorApp::GetGraph() const
{
	if (UTKGraph)
	{
		return UTKGraph;
	}

	return nullptr;
}

void FUTKEditorApp::OnClose()
{
	FUTKGraphBuilder::SaveToAsset(UTKGraph, EditingObject);

	if (UTKGraph)
		UTKGraph->RemoveOnGraphChangedHandler(GraphChangedListenerHandle);

	GEditor->UnregisterForUndo(this);

	FWorkflowCentricApplication::OnClose();
}

void FUTKEditorApp::OnGraphChanged(const FEdGraphEditAction& Action)
{
	if (!UTKGraph || !EditingObject)
		return;

	FUTKGraphBuilder::SaveToAsset(UTKGraph, EditingObject);
}

void FUTKEditorApp::DeleteSelectedNodes()
{
	if (!GraphUI.IsValid())
		return;

	const FGraphPanelSelectionSet SelectedNodes = GraphUI->GetSelectedNodes();
	if (SelectedNodes.Num() == 0)
		return;

	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEditor_DeleteNode", "Delete Nodes"));
	UTKGraph->Modify();

	for (UObject* NodeObj : SelectedNodes)
	{
		if (UUTKNode* Node = Cast<UUTKNode>(NodeObj))
		{
			Node->Modify();
			Node->DestroyNode();
		}
	}

	GraphUI->ClearSelectionSet();
}

bool FUTKEditorApp::CanDeleteNodes()
{
	return GraphUI.IsValid()
		&& GraphUI->GetSelectedNodes().Num() > 0;

}

void FUTKEditorApp::Undo()
{
	GEditor->UndoTransaction();
	if (GraphUI.IsValid())
	{
		GraphUI->NotifyGraphChanged();
	}
}

void FUTKEditorApp::Redo()
{
	GEditor->RedoTransaction();
	if (GraphUI.IsValid())
	{
		GraphUI->NotifyGraphChanged();
	}
}

void FUTKEditorApp::PostUndo(bool bSuccess)
{
	if (GraphUI.IsValid())
	{
		GraphUI->NotifyGraphChanged();
	}
}

void FUTKEditorApp::PostRedo(bool bSuccess)
{
	FEditorUndoClient::PostRedo(bSuccess);
}

#undef LOCTEXT_NAMESPACE