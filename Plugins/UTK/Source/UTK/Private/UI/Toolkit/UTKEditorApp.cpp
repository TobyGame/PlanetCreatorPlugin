#include "UI/Toolkit/UTKEditorApp.h"
#include "UI/Toolkit/UTKEditorMode.h"
#include "Modules/ModuleManager.h"
#include "Assets/UTKAsset.h"
#include "Framework/Commands/GenericCommands.h"
#include "Graph/UTKGraph.h"
#include "Graph/UTKGraphSchema.h"
#include "Graph/Nodes/UTKNode.h"

#define LOCTEXT_NAMESPACE "UTKEditor"

void FUTKEditorApp::InitUTKEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* UTKAsset)
{
	EditingObject = Cast<UUTKAsset>(UTKAsset);

	if (!EditingObject.Get())
		return;

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

	bSuppressChangeNotifications = true;

	InitializeWorkingAsset(EditingObject.Get());

	if (UTKGraph.Get())
	{
		GraphChangedListenerHandle = UTKGraph->AddOnGraphChangedHandler(
			FOnGraphChanged::FDelegate::CreateRaw(this, &FUTKEditorApp::OnWorkingGraphChanged));
	}

	ObjectPropChangedHandle = FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(
		this,
		&FUTKEditorApp::OnWorkingObjectPropertyChanged);

	TransactionHandle = FCoreUObjectDelegates::OnObjectTransacted.AddRaw(
		this,
		&FUTKEditorApp::OnObjectTransected);

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

	bSuppressChangeNotifications = false;

	bWorkingDirty = false;
}

void FUTKEditorApp::InitializeWorkingAsset(UUTKAsset* InOriginal)
{
	WorkingObject = Cast<UUTKAsset>(StaticDuplicateObject(
		InOriginal,
		GetTransientPackage(),
		NAME_None,
		~RF_Standalone,
		InOriginal->GetClass()));

	if (WorkingObject.Get())
	{
		WorkingObject->SetFlags(RF_Transactional);

		if (!WorkingObject->Graph)
		{
			WorkingObject->Graph = NewObject<UUTKGraph>(WorkingObject.Get(),
				UUTKGraph::StaticClass(),
				NAME_None,
				RF_Transactional);

			WorkingObject->Graph->Schema = UUTKGraphSchema::StaticClass();
		}
		else if (!WorkingObject->Graph->Schema)
		{
			WorkingObject->Graph->Schema = UUTKGraphSchema::StaticClass();
		}

		UTKGraph = WorkingObject->Graph;
	}

	bWorkingDirty = false;
}

UUTKGraph* FUTKEditorApp::GetGraph() const
{
	return UTKGraph.Get();
}

void FUTKEditorApp::OnWorkingGraphChanged(const FEdGraphEditAction& Action)
{
	if (bSuppressChangeNotifications) return;

	bWorkingDirty = true;
	// Update toolkit UI (title/tab) to reflect dirty state; RegenerateMenusAndToolbars is
	// available on FAssetEditorToolkit/FWorkflowCentricApplication and will trigger a UI refresh.
	RegenerateMenusAndToolbars();
}

void FUTKEditorApp::OnWorkingObjectPropertyChanged(UObject* Object, struct FPropertyChangedEvent& Event)
{
	if (bSuppressChangeNotifications) return;

	if (!WorkingObject.Get() || !Object) return;

	if (Object == WorkingObject.Get() || Object->IsIn(WorkingObject.Get()))
	{
		bWorkingDirty = true;
		RegenerateMenusAndToolbars();
	}
}

void FUTKEditorApp::OnObjectTransected(UObject* Object, const class FTransactionObjectEvent& Event)
{
	if (bSuppressChangeNotifications) return;

	if (!WorkingObject.Get() || !Object) return;

	if (Object == WorkingObject.Get() || Object->IsIn(WorkingObject.Get()))
	{
		bWorkingDirty = true;
		RegenerateMenusAndToolbars();
	}
}

static void CopyEditableProps(UObject* From, UObject* To)
{
	UClass* Cls = To->GetClass();
	for (TFieldIterator<FProperty> It(Cls, EFieldIterationFlags::IncludeSuper); It; ++It)
	{
		FProperty* Prop = *It;

		const FName PName = Prop->GetFName();
		if (PName == TEXT("Graph")) { continue; }
		if (!Prop->HasAnyPropertyFlags(CPF_Edit)) { continue; }
		if (Prop->HasAnyPropertyFlags(CPF_Transient)) { continue; }

		Prop->CopyCompleteValue_InContainer(To, From);
	}
}

void FUTKEditorApp::ApplyWorkingToOriginal()
{
	if (!EditingObject.Get() || !WorkingObject.Get()) return;

	// Apply changes to the original asset (wrapped in Modify() so Undo/Redo works).
	EditingObject->Modify();

	CopyEditableProps(WorkingObject.Get(), EditingObject.Get());

	if (WorkingObject->Graph)
	{
		if (EditingObject->Graph)
		{
			EditingObject->Graph->Modify();
			EditingObject->Graph->ClearFlags(RF_Public | RF_Standalone);
			// Rename returns a value; cast to void to avoid "expression result is not used" warnings.
			EditingObject->Graph->Rename(nullptr,
				GetTransientPackage(),
				REN_DontCreateRedirectors | REN_NonTransactional);
		}

		UUTKGraph* NewGraph = DuplicateObject<UUTKGraph>(WorkingObject->Graph, EditingObject.Get());
		if (NewGraph)
		{
			NewGraph->SetFlags(RF_Transactional);
			if (!NewGraph->Schema)
			{
				NewGraph->Schema = UUTKGraphSchema::StaticClass();
			}
			EditingObject->Graph = NewGraph;
		}
	}

	EditingObject->MarkPackageDirty();

	bWorkingDirty = false;
	// Refresh toolkit UI to remove dirty indicator when changes are applied.
	RegenerateMenusAndToolbars();
}

void FUTKEditorApp::SaveAsset_Execute()
{
	// If working copy has changes, apply them first so the original assets are up-to-date.
	if (bWorkingDirty)
	{
		ApplyWorkingToOriginal();
	}

	// Let the base toolkit perform save handling (it will call GetSaveableObjects and the editor
	// saving helpers as appropriate). We keep our override minimal to follow engine editors.
	FWorkflowCentricApplication::SaveAsset_Execute();
}


bool FUTKEditorApp::OnRequestClose(EAssetEditorCloseReason InCloseReason)
{
	if (!bWorkingDirty)
		return true;

	const EAppReturnType::Type Reply = FMessageDialog::Open(
		EAppMsgType::YesNoCancel,
		LOCTEXT("UTK_ClosePrompt", "Save changes to this asset before closing?"));

	switch (Reply)
	{
	case EAppReturnType::Yes:
		SaveAsset_Execute();
		return true;

	case EAppReturnType::No:
		return true;

	default:
		return false;
	}
}

void FUTKEditorApp::OnClose()
{
	if (UTKGraph.Get() && GraphChangedListenerHandle.IsValid())
	{
		UTKGraph->RemoveOnGraphChangedHandler(GraphChangedListenerHandle);
		GraphChangedListenerHandle.Reset();
	}

	if (ObjectPropChangedHandle.IsValid())
	{
		FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(ObjectPropChangedHandle);
		ObjectPropChangedHandle.Reset();
	}

	if (TransactionHandle.IsValid())
	{
		FCoreUObjectDelegates::OnObjectTransacted.Remove(TransactionHandle);
		TransactionHandle.Reset();
	}

	GEditor->UnregisterForUndo(this);

	FWorkflowCentricApplication::OnClose();
}

void FUTKEditorApp::GetSaveableObjects(TArray<UObject*>& OutObjects) const
{
	if (!EditingObject.Get())
	{
		return;
	}

	if (bWorkingDirty)
	{
		OutObjects.Add(const_cast<UUTKAsset*>(EditingObject.Get()));
		return;
	}
	if (UPackage* Package = EditingObject->GetOutermost())
	{
		if (Package->IsDirty())
		{
			OutObjects.Add(const_cast<UUTKAsset*>(EditingObject.Get()));
		}
	}
}

void FUTKEditorApp::DeleteSelectedNodes()
{
	if (!GraphUI.IsValid())
		return;

	const FGraphPanelSelectionSet SelectedNodes = GraphUI->GetSelectedNodes();
	if (SelectedNodes.Num() == 0)
		return;

	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "UTK_DeleteNode", "Delete Nodes"));
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

FText FUTKEditorApp::GetToolkitName() const
{
	const FText BaseName = GetBaseToolkitName();

	if (EditingObject.Get())
	{
		if (bWorkingDirty)
		{
			return FText::FromString(BaseName.ToString() + TEXT(" *"));
		}
		if (UPackage* Package = EditingObject->GetOutermost())
		{
			if (Package->IsDirty())
			{
				return FText::FromString(BaseName.ToString() + TEXT(" *"));
			}
		}
	}

	return BaseName;
}

void FUTKEditorApp::OnGraphSelectionChanged(const TSet<UObject*>& NewSelection)
{
	UUTKNode* NewSelectedNode = nullptr;

	// Simple node for now:
	// - If exactly one node is selected, we track that node.
	// - If multiple objects are selected, we pick the first UTKNode we find.
	// - If nothing relevant is selected, we clear the selection.
	if (NewSelection.Num() == 1)
	{
		auto It = NewSelection.begin();
		if (It)
		{
			NewSelectedNode = Cast<UUTKNode>(*It);
		}
	}
	else if (NewSelection.Num() > 1)
	{
		for (UObject* Object : NewSelection)
		{
			if (UUTKNode* AsNode = Cast<UUTKNode>(Object))
			{
				NewSelectedNode = AsNode;
				break;
			}
		}
	}

	UUTKNode* OldNode = SelectedNode.Get();
	SelectedNode = NewSelectedNode;

	if (OldNode != NewSelectedNode)
	{
		SelectedNodeChanged.Broadcast(NewSelectedNode);
	}
}

#undef LOCTEXT_NAMESPACE