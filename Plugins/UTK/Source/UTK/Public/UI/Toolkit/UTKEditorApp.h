#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"
#include "Core/UTKTerrainTypes.h"

class UTexture2D;
class UUTKAsset;
class UUTKGraph;
class FUTKEditorMode;
class UUTKNode;
class UUTKEditorPreviewSettings;

DECLARE_MULTICAST_DELEGATE_OneParam(FUTKOnSelectedNodeChanged, UUTKNode*)

/**
 * Core class for the UTK Generator Tool Editor
 */
class FUTKEditorApp : public FWorkflowCentricApplication
                      , public FEditorUndoClient
{
public:
	void InitUTKEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* UTKAsset);

	virtual FName GetToolkitFName() const override { return FName("UTKEditor"); }
	virtual FText GetBaseToolkitName() const override { return FText::FromString("UTK Editor"); }
	virtual FText GetToolkitName() const;
	virtual FString GetWorldCentricTabPrefix() const override { return TEXT("UTKEditor"); }
	virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor::White; }

	static TSharedPtr<FUTKEditorApp> GetLastInstance();

	UUTKGraph* GetGraph() const;
	UUTKAsset* GetWorkingAsset() const { return WorkingObject.Get(); }
	void SetWorkingGraphUI(TSharedPtr<SGraphEditor> InGrapUI) { GraphUI = InGrapUI; }
	int32 GetPreviewResolution() const;
	int32 GetPreviewSeed() const;

	TSharedPtr<FUTKTerrain> EvaluatePreview(int32 ResolutionX, int32 ResolutionY, int32 Seed, FName& OutPreviewLayerName);
	void UpdatePreviewTexture(const TSharedPtr<FUTKTerrain>& Terrain, FName LayerName);
	UTexture2D* GetPreviewTexture() const { return PreviewTexture; }
	void EvaluateCurrentSelectionForPreview();
	FName GetPreviewOutputPinOverrideForNode(const UUTKNode* Node) const;
	void SetPreviewOutputPinOverrideForNode(const UUTKNode* Node, FName OutputPinName);

	UUTKNode* GetSelectedNode() const { return SelectedNode.Get(); }
	FUTKOnSelectedNodeChanged& OnSelectedNodeChanged() { return SelectedNodeChanged; }
	void OnGraphSelectionChanged(const TSet<UObject*>& NewSelection);

	virtual void OnClose() override;
	virtual bool OnRequestClose(EAssetEditorCloseReason InCloseReason) override;
	virtual void SaveAsset_Execute() override;
	virtual void GetSaveableObjects(TArray<UObject*>& OutObjects) const override;

	// Deletion logic
	void DeleteSelectedNodes();
	bool CanDeleteNodes();

	// Undo/Redo system
	virtual void Undo();
	virtual void Redo();
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;

	// -------- Evaluation / caching info -----------
	uint64 GetGraphRevision() const { return GraphRevision; }
	uint64 GetPreviewRevision() const { return PreviewRevision; }
	UUTKNode* GetFocusedNode() const { return FocusedNode.Get(); }
	void SetFocusedNode(UUTKNode* InNode);
	void MarkGraphDirty();
	void MarkPreviewSettingsChanged();

private:
	TObjectPtr<UUTKAsset> EditingObject = nullptr;
	TObjectPtr<UUTKAsset> WorkingObject = nullptr;
	TObjectPtr<UUTKGraph> UTKGraph = nullptr;

	TSharedPtr<FUTKEditorMode> UTKEditorMode;
	TSharedPtr<SGraphEditor> GraphUI = nullptr;
	TSharedPtr<FUICommandList> CommandList;

	TWeakObjectPtr<UUTKNode> SelectedNode;
	FUTKOnSelectedNodeChanged SelectedNodeChanged;

	bool bWorkingDirty = false;
	bool bIsClosing = false;

	TMap<FGuid, FName> PreviewOutputPinOverrides;

protected:
	uint64 GraphRevision = 0;
	uint64 PreviewRevision = 0;
	TWeakObjectPtr<UUTKNode> FocusedNode;

	// Not a UPROPERTY — this class is not a UObject.
	TObjectPtr<UTexture2D> PreviewTexture = nullptr;

	// When true, change/transaction/property notifications are ignored. Used during initialization
	// to avoid marking the working asset dirty because of editor-side transient setup operations.
	bool bSuppressChangeNotifications = false;

	FDelegateHandle GraphChangedListenerHandle;
	FDelegateHandle ObjectPropChangedHandle;
	FDelegateHandle TransactionHandle;

private:
	void InitializeWorkingAsset(UUTKAsset* InOriginal);
	void OnWorkingGraphChanged(const FEdGraphEditAction& Action);
	void OnWorkingObjectPropertyChanged(UObject* Object, FPropertyChangedEvent& Event);
	void OnObjectTransected(UObject* Object, const FTransactionObjectEvent& Event);
	void ApplyWorkingToOriginal();
	uint32 ComputeWorkingGraphConnectionHash() const;

	static TWeakPtr<FUTKEditorApp> LastInstance;
	uint32 CachedGraphConnectionHash = 0;
	bool bHasCachedGraphConnectionHash = false;
};