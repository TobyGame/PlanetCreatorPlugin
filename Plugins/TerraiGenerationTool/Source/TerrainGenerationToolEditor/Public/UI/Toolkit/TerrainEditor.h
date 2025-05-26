#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

class UTerrainAsset;
class UTerrainGraph;
class FTerrainEditorMode;

/**
 * Core class for the Terrain Generator Tool Editor
 */
class FTerrainEditor : public FWorkflowCentricApplication, public FEditorUndoClient, public FNotifyHook
{
public:
	void InitTerrainEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* TerrainAsset);

	virtual FName GetToolkitFName() const override { return FName("TerrainEditor"); }
	virtual FText GetBaseToolkitName() const override { return FText::FromString("Terrain Editor"); }
	virtual FString GetWorldCentricTabPrefix() const override { return TEXT("TerrainEditor"); }
	virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor::White; }

	UTerrainAsset* GetEditingAsset() const { return EditingObject; }
	UTerrainGraph* GetGraph() const;
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
	UTerrainAsset* EditingObject = nullptr;
	UTerrainGraph* TerrainGraph = nullptr;
	TSharedPtr<FTerrainEditorMode> TerrainEditorMode;
	TSharedPtr<SGraphEditor> GraphUI = nullptr;
	TSharedPtr<FUICommandList> CommandList;

	FDelegateHandle GraphChangedListenerHandle;
};