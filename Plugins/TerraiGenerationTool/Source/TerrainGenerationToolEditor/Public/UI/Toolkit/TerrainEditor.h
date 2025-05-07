#pragma once

#include "CoreMinimal.h"
#include "Graph/TerrainGraph.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

class UTerrainAsset;
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
	UTerrainAsset* GetEditingAsset() const { return EditingObject; }
	virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor::White; }

	UTerrainGraph* GetGraph() const;

private:
	UTerrainAsset* EditingObject = nullptr;
	UTerrainGraph* TerrainGraph = nullptr;
	TSharedPtr<FTerrainEditorMode> TerrainEditorMode;
};