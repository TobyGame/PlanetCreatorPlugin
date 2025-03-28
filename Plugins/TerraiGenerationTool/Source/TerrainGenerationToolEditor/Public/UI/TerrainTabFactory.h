#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"

class FTerrainEditor;

/**
 * Base class for Terrain Editor Tabs
 */
class FTerrainTabFactory : public FWorkflowTabFactory
{
public:
	FTerrainTabFactory(FName InTabID, TSharedPtr<FTerrainEditor> InEditor);

protected:
	TWeakPtr<FTerrainEditor> TerrainEditor;
};