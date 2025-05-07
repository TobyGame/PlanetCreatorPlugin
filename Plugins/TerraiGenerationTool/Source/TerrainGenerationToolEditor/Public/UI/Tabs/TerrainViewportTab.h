#pragma once

#include "Toolkit/TerrainTabFactory.h"

class FTerrainViewportTab : public FTerrainTabFactory
{
public:
	FTerrainViewportTab(TSharedPtr<FTerrainEditor> InEditor);
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;

private:
	TWeakPtr<FTerrainEditor> Editor;
};