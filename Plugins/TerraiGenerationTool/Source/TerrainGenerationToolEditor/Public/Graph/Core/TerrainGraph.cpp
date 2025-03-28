#include "Graph/Core/TerrainGraph.h"
#include "Graph/Core/TerrainGraphSchema.h"

UTerrainGraph::UTerrainGraph()
{
	Schema = UTerrainGraphSchema::StaticClass();
}