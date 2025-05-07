#include "Graph/TerrainGraph.h"
#include "Graph/TerrainGraphSchema.h"

UTerrainGraph::UTerrainGraph()
{
	Schema = UTerrainGraphSchema::StaticClass();
}