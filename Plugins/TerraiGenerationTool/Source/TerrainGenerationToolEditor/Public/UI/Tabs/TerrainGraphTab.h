#pragma once

#include "Toolkit/TerrainTabFactory.h"

class FTerrainGraphTab : public FTerrainTabFactory
{
public:
	FTerrainGraphTab(TSharedPtr<FTerrainEditor> InEditor);
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;

private:
	TWeakPtr<FTerrainEditor> Editor;
};