#pragma once

#include "TerrainTabFactory.h"

class FTerrainGeneralPropertiesTab : public FTerrainTabFactory
{
public:
	FTerrainGeneralPropertiesTab(TSharedPtr<FTerrainEditor> InEditor);
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
};