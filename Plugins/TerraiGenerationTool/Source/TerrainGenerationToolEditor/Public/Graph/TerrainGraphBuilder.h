#pragma once

#include "CoreMinimal.h"
#include "Core/TerrainGraphSaveData.h"

class UTerrainAsset;
class UTerrainGraph;

class FTerrainGraphBuilder
{
public:
	static void LoadFromAsset(UTerrainAsset* Asset, UTerrainGraph* Graph);
	static void SaveToAsset(UTerrainGraph* Graph, UTerrainAsset* Asset);
};