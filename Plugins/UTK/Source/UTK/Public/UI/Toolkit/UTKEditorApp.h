#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

class UUTKAsset;
class UUTKGraph;
class FUTKEditorMode;

/**
 * Core class for the UTK Generator Tool Editor
 */
class FUTKEditorApp : public FWorkflowCentricApplication
                      , public FEditorUndoClient
                      , public FNotifyHook
{
public:
	void InitUTKEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* UTKAsset);

	virtual FName GetToolkitFName() const override { return FName("UTKEditor"); }
	virtual FText GetBaseToolkitName() const override { return FText::FromString("UTK Editor"); }
	virtual FText GetToolkitName() const;
	virtual FString GetWorldCentricTabPrefix() const override { return TEXT("UTKEditor"); }
	virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor::White; }

	UUTKGraph* GetGraph() const;
	UUTKAsset* GetWorkingAsset() const { return WorkingObject.Get(); }
	void SetWorkingGraphUI(TSharedPtr<SGraphEditor> InGrapUI) { GraphUI = InGrapUI; }

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

private:
	TObjectPtr<UUTKAsset> EditingObject = nullptr;
	TObjectPtr<UUTKAsset> WorkingObject = nullptr;
	TObjectPtr<UUTKGraph> UTKGraph = nullptr;

	TSharedPtr<FUTKEditorMode> UTKEditorMode;
	TSharedPtr<SGraphEditor> GraphUI = nullptr;
	TSharedPtr<FUICommandList> CommandList;

	bool bWorkingDirty = false;

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
};