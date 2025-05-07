#pragma once

#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

class FTerrainEditor;

/**
 * Terrain Editor UI Mode - Manage layout and tab registration
 */
class FTerrainEditorMode : public FApplicationMode
{
public:
	FTerrainEditorMode(TSharedPtr<FTerrainEditor> InEditor);

	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void PreDeactivateMode() override;
	virtual void PostActivateMode() override;

private:
	TWeakPtr<FTerrainEditor> Editor;
	FWorkflowAllowedTabSet TabsSet;
};