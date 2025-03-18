#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

class FTerrainEditorMode;

/**
 * Core class for the Terrain Generator Tool Editor
 */
class FTerrainEditor : public FWorkflowCentricApplication
{
public:
	void InitTerrainEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* TerrainAsset);

	virtual FName GetToolkitFName() const override { return FName("TerrainEditor"); }
	virtual FText GetBaseToolkitName() const override { return FText::FromString("Terrain Editor"); }
	virtual FString GetWorldCentricTabPrefix() const override { return TEXT("TerrainEditor"); }
	UObject* GetEdittingObject() const { return EditingObject; }

protected:
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;

private:
	UObject* EditingObject;
	TSharedPtr<FTerrainEditorMode> TerrainEditorMode;
};