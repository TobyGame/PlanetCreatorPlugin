#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

class FUTKEditorApp;

/**
 * UTK Editor UI Mode - Manage layout and tab registration
 */
class FUTKEditorMode : public FApplicationMode
{
public:
	FUTKEditorMode(TSharedPtr<FUTKEditorApp> InEditor);

	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void PreDeactivateMode() override;
	virtual void PostActivateMode() override;

private:
	TWeakPtr<FUTKEditorApp> Editor;
	FWorkflowAllowedTabSet TabsSet;
};