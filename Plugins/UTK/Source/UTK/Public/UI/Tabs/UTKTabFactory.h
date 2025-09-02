#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"

class FUTKEditorApp;

/**
 * Base class for UTK Editor Tabs
 */
class FUTKTabFactory : public FWorkflowTabFactory
{
public:
	FUTKTabFactory(FName InTabID, TSharedPtr<FUTKEditorApp> InEditor);

protected:
	TWeakPtr<FUTKEditorApp> UTKEditor;
};