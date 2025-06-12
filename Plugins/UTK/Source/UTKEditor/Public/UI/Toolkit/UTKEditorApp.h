#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

class UUTKAsset;
class UUTKGraph;
class FUTKEditorMode;

/**
 * Core class for the UTK Generator Tool Editor
 */
class FUTKEditorApp : public FWorkflowCentricApplication, public FEditorUndoClient, public FNotifyHook
{
public:
	void InitUTKEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* UTKAsset);

	virtual FName GetToolkitFName() const override { return FName("UTKEditor"); }
	virtual FText GetBaseToolkitName() const override { return FText::FromString("UTK Editor"); }
	virtual FString GetWorldCentricTabPrefix() const override { return TEXT("UTKEditor"); }
	virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor::White; }

	UUTKAsset* GetEditingAsset() const { return EditingObject; }
	UUTKGraph* GetGraph() const;
	void SetWorkingGraphUI(TSharedPtr<SGraphEditor> InGrapUI) { GraphUI = InGrapUI; }

	virtual void OnClose() override;
	void OnGraphChanged(const FEdGraphEditAction& Action);

	// Deletion logic
	void DeleteSelectedNodes();
	bool CanDeleteNodes();

	// Undo/Redo system
	virtual void Undo();
	virtual void Redo();
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;

private:
	UUTKAsset* EditingObject = nullptr;
	UUTKGraph* UTKGraph = nullptr;
	TSharedPtr<FUTKEditorMode> UTKEditorMode;
	TSharedPtr<SGraphEditor> GraphUI = nullptr;
	TSharedPtr<FUICommandList> CommandList;

	FDelegateHandle GraphChangedListenerHandle;
};