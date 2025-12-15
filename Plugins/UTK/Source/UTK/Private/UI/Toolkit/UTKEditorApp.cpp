#include "UI/Toolkit/UTKEditorApp.h"
#include "UI/Toolkit/UTKEditorMode.h"
#include "Modules/ModuleManager.h"
#include "Assets/UTKAsset.h"
#include "Framework/Commands/GenericCommands.h"
#include "Graph/UTKGraph.h"
#include "Graph/UTKGraphSchema.h"
#include "Graph/Nodes/UTKNodeSettings.h"
#include "Graph/Nodes/UTKNode.h"
#include "Graph/UTKGraphEvaluation.h"

#define LOCTEXT_NAMESPACE "UTKEditor"

TWeakPtr<FUTKEditorApp> FUTKEditorApp::LastInstance;

void FUTKEditorApp::InitUTKEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* UTKAsset)
{
	LastInstance = SharedThis(this);

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

	CachedGraphConnectionHash = ComputeWorkingGraphConnectionHash();
	bHasCachedGraphConnectionHash = true;

	bWorkingDirty = false;
}

TSharedPtr<FUTKEditorApp> FUTKEditorApp::GetLastInstance()
{
	return LastInstance.Pin();
}

void FUTKEditorApp::SetFocusedNode(UUTKNode* InNode)
{
	FocusedNode = InNode;
}

void FUTKEditorApp::MarkGraphDirty()
{
	++GraphRevision;

	bWorkingDirty = true;
	RegenerateMenusAndToolbars();
}

void FUTKEditorApp::MarkPreviewSettingsChanged()
{
	++PreviewRevision;
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
		PreviewOutputPinOverrides.Empty();

		CachedGraphConnectionHash = 0;
		bHasCachedGraphConnectionHash = false;
	}

	bWorkingDirty = false;
}

UUTKGraph* FUTKEditorApp::GetGraph() const
{
	return UTKGraph.Get();
}

uint32 FUTKEditorApp::ComputeWorkingGraphConnectionHash() const
{
	if (!UTKGraph.Get())
		return 0;

	TArray<uint32> EdgeHashes;
	EdgeHashes.Reserve(256);

	for (UEdGraphNode* Node : UTKGraph->Nodes)
	{
		if (!Node)
			continue;

		const FGuid NodeGuid = Node->NodeGuid;

		for (UEdGraphPin* Pin : Node->Pins)
		{
			if (!Pin)
				continue;

			if (Pin->Direction != EGPD_Input)
				continue;

			if (Pin->LinkedTo.Num() <= 0 || !Pin->LinkedTo[0])
				continue;

			UEdGraphPin* Linked = Pin->LinkedTo[0];
			UEdGraphNode* UpNode = Linked->GetOwningNode();

			if (!UpNode)
				continue;

			const FGuid UpGuid = UpNode->NodeGuid;

			uint32 H = 0;
			H = HashCombine(H, GetTypeHash(UpGuid));
			H = HashCombine(H, GetTypeHash(Linked->PinName));
			H = HashCombine(H, GetTypeHash(NodeGuid));
			H = HashCombine(H, GetTypeHash(Pin->PinName));

			EdgeHashes.Add(H);
		}
	}

	Algo::Sort(EdgeHashes);

	uint32 FinalHash = 0;
	for (uint32 H : EdgeHashes)
		FinalHash = HashCombine(FinalHash, H);

	return FinalHash;
}

static bool GetPrimaryOutputPinFromDefinition(UUTKNode* Node, FName& OutPinName, FName& OutLayerName)
{
	if (!Node)
		return false;

	const FUTKNodeDefinition& Def = Node->GetDefinition();

	for (const FUTKNodePinDefinition& Pin : Def.Pins)
	{
		if (!Pin.bInput)
		{
			OutPinName = Pin.Name;
			OutLayerName = Pin.DefaultLayerName;
			return true;
		}
	}

	return false;
}

FName FUTKEditorApp::GetPreviewOutputPinOverrideForNode(const UUTKNode* Node) const
{
	if (!Node)
		return NAME_None;

	if (const FName* Found = PreviewOutputPinOverrides.Find(Node->NodeGuid))
		return *Found;

	return NAME_None;
}

void FUTKEditorApp::SetPreviewOutputPinOverrideForNode(const UUTKNode* Node, FName OutputPinName)
{
	if (!Node)
		return;

	if (OutputPinName.IsNone())
		PreviewOutputPinOverrides.Remove(Node->NodeGuid);
	else
		PreviewOutputPinOverrides.Add(Node->NodeGuid, OutputPinName);
}

TSharedPtr<FUTKTerrain> FUTKEditorApp::EvaluatePreview(int32 ResolutionX, int32 ResolutionY, int32 Seed, FName& OutPreviewLayerName)
{
	UUTKNode* PreviewNode = nullptr;

	if (FocusedNode.IsValid())
		PreviewNode = FocusedNode.Get();
	else
		PreviewNode = SelectedNode.Get();

	if (!PreviewNode)
		return nullptr;

	const FName OverridePinName = GetPreviewOutputPinOverrideForNode(PreviewNode);
	const FUTKNodeDefinition& Definition = PreviewNode->GetDefinition();

	FName OutputPinName;
	FName LayerName;

	bool bFoundPin = false;

	if (!OverridePinName.IsNone())
	{
		for (const FUTKNodePinDefinition& Pin : Definition.Pins)
		{
			if (!Pin.bInput && Pin.Name == OverridePinName)
			{
				OutputPinName = Pin.Name;
				LayerName = Pin.DefaultLayerName;
				bFoundPin = true;
				break;
			}
		}

		if (!bFoundPin)
			const_cast<FUTKEditorApp*>(this)->SetPreviewOutputPinOverrideForNode(PreviewNode, NAME_None);
	}

	if (!bFoundPin)
	{
		if (!GetPrimaryOutputPinFromDefinition(PreviewNode, OutputPinName, LayerName))
			return nullptr;
	}

	FUTKNodeExecutionContext Ctx(ResolutionX, ResolutionY, Seed);
	Ctx.GraphRevision = GraphRevision;
	Ctx.PreviewRevision = PreviewRevision;

	TSharedPtr<FUTKTerrain> Terrain = EvaluateNodeOutput(PreviewNode, OutputPinName, Ctx);

	OutPreviewLayerName = LayerName;
	return Terrain;
}

void FUTKEditorApp::UpdatePreviewTexture(const TSharedPtr<FUTKTerrain>& Terrain, FName LayerName)
{
	if (!Terrain.IsValid() || !Terrain->IsValid())
	{
		PreviewTexture = nullptr;
		return;
	}

	const FUTKLayer* Layer = Terrain->FindLayer(LayerName);

	if (!Layer)
	{
		Layer = Terrain->FindLayer(TEXT("Height"));

		if (!Layer)
		{
			PreviewTexture = nullptr;
			return;
		}
	}

	const FUTKBuffer2D& Buffer = *Layer->Data;

	const int32 Width = Buffer.Width;
	const int32 Height = Buffer.Height;

	if (Width <= 0 || Height <= 0)
	{
		PreviewTexture = nullptr;
		return;
	}

	float MinValue = TNumericLimits<float>::Max();
	float MaxValue = TNumericLimits<float>::Lowest();

	for (int32 Y = 0; Y < Height; ++Y)
	{
		for (int32 X = 0; X < Width; ++X)
		{
			const float V = Buffer.Get(X, Y);

			MinValue = FMath::Min(MinValue, V);
			MaxValue = FMath::Max(MaxValue, V);
		}
	}

	if (!FMath::IsFinite(MinValue) || !FMath::IsFinite(MaxValue) || MinValue >= MaxValue)
	{
		MinValue = 0.0f;
		MaxValue = 1.0f;
	}

	const float RangeInv = 1.0f / (MaxValue - MinValue);

	const bool bNeedsNewTexture =
		!PreviewTexture ||
		PreviewTexture->GetSizeX() != Width ||
		PreviewTexture->GetSizeY() != Height;

	if (bNeedsNewTexture)
	{
		PreviewTexture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
		PreviewTexture->MipGenSettings = TMGS_NoMipmaps;
		PreviewTexture->SRGB = false;
	}

	FTexture2DMipMap& Mip = PreviewTexture->GetPlatformData()->Mips[0];

	void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
	FColor* Dest = static_cast<FColor*>(Data);

	for (int32 Y = 0; Y < Height; ++Y)
	{
		for (int32 X = 0; X < Width; ++X)
		{
			const float V = Buffer.Get(X, Y);
			const float Normalized = FMath::Clamp((V - MinValue) * RangeInv, 0.0f, 1.0f);
			const uint8 Gray = static_cast<uint8>(Normalized * 255.0f + 0.5f);

			Dest[Y * Width + X] = FColor(Gray, Gray, Gray, 255);
		}
	}

	Mip.BulkData.Unlock();
	PreviewTexture->UpdateResource();
}

void FUTKEditorApp::EvaluateCurrentSelectionForPreview()
{
	UUTKNode* PreviewNode = nullptr;

	if (FocusedNode.IsValid())
		PreviewNode = FocusedNode.Get();
	else
		PreviewNode = SelectedNode.Get();

	if (!PreviewNode)
	{
		PreviewNode = nullptr;
		return;
	}

	const int32 PreviewRes = GetPreviewResolution();
	const int32 PreviewSeed = GetPreviewSeed();

	FName PreviewLayerName(NAME_None);

	TSharedPtr<FUTKTerrain> Terrain =
		EvaluatePreview(PreviewRes, PreviewRes, PreviewSeed, PreviewLayerName);

	if (!Terrain.IsValid())
	{
		PreviewTexture = nullptr;
		return;
	}

	if (PreviewLayerName.IsNone())
		PreviewLayerName = TEXT("Height");

	UpdatePreviewTexture(Terrain, PreviewLayerName);
}

void FUTKEditorApp::OnWorkingGraphChanged(const FEdGraphEditAction& Action)
{
	if (bSuppressChangeNotifications) return;

	MarkGraphDirty();

	const uint32 NewHash = ComputeWorkingGraphConnectionHash();

	if (!bHasCachedGraphConnectionHash || NewHash != CachedGraphConnectionHash)
	{
		CachedGraphConnectionHash = NewHash;
		bHasCachedGraphConnectionHash = true;

		if (SelectedNode.IsValid())
			EvaluateCurrentSelectionForPreview();
	}
}

void FUTKEditorApp::OnWorkingObjectPropertyChanged(UObject* Object, struct FPropertyChangedEvent& Event)
{
	if (bSuppressChangeNotifications) return;

	if (!WorkingObject.Get() || !Object) return;

	bool bShouldReevaluatePreview = false;

	if (UUTKNodeSettings* Settings = Cast<UUTKNodeSettings>(Object))
	{
		if (UUTKNode* Selected = SelectedNode.Get())
		{
			if (Selected->GetSettings() == Settings)
			{
				Selected->InvalidateCache();
				bShouldReevaluatePreview = true;
			}
		}
	}

	if (Object == WorkingObject.Get())
	{
		if (Event.Property)
		{
			const FName PropName = Event.Property->GetFName();

			if (PropName == GET_MEMBER_NAME_CHECKED(UUTKAsset, PreviewResolution) ||
				PropName == GET_MEMBER_NAME_CHECKED(UUTKAsset, PreviewSeed))
			{
				MarkPreviewSettingsChanged();
				bShouldReevaluatePreview = true;
			}
		}
	}

	if (bShouldReevaluatePreview)
		EvaluateCurrentSelectionForPreview();

	if (Object == WorkingObject.Get() || Object->IsIn(WorkingObject.Get()))
	{
		MarkGraphDirty();
	}
}

void FUTKEditorApp::OnObjectTransected(UObject* Object, const class FTransactionObjectEvent& Event)
{
	if (bSuppressChangeNotifications) return;

	if (!WorkingObject.Get() || !Object) return;

	if (Object == WorkingObject.Get() || Object->IsIn(WorkingObject.Get()))
	{
		MarkGraphDirty();
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
		EvaluateCurrentSelectionForPreview();
	}
}

int32 FUTKEditorApp::GetPreviewResolution() const
{
	const int32 DefaultRes = 512;

	if (UUTKAsset* Asset = GetWorkingAsset())
	{
		if (Asset->PreviewResolution > 0)
			return Asset->PreviewResolution;
	}

	return DefaultRes;
}

int32 FUTKEditorApp::GetPreviewSeed() const
{
	const int32 DefaultSeed = 0;

	if (UUTKAsset* Asset = GetWorkingAsset())
	{
		if (Asset->PreviewResolution >= 0)
			return Asset->PreviewSeed;
	}

	return DefaultSeed;
}

#undef LOCTEXT_NAMESPACE