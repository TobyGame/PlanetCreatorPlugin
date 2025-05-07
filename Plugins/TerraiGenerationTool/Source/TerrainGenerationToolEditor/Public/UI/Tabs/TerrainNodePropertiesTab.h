#pragma once

#include "Toolkit/TerrainTabFactory.h"


class FTerrainNodePropertiesTab : public FTerrainTabFactory
{
public:
	FTerrainNodePropertiesTab(TSharedPtr<FTerrainEditor> InEditor);
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;

private:
	TWeakPtr<FTerrainEditor> Editor;
};