#pragma once

#include "TerrainTabFactory.h"

class FTerrainLoggerTab : public FTerrainTabFactory
{
public:
	FTerrainLoggerTab(TSharedPtr<FTerrainEditor> InEditor);
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
};