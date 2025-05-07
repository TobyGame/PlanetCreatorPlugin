#include "Graph/Nodes/TerrainNodeFactory.h"

FTerrainNodeFactory& FTerrainNodeFactory::Get()
{
	static FTerrainNodeFactory Instance;
	return Instance;
}

void FTerrainNodeFactory::RegisterNode(const FTerrainNodeDefinition& Definition)
{
	NodeMap.Add(Definition.Name, Definition);
}

const TMap<FString, FTerrainNodeDefinition>& FTerrainNodeFactory::GetAllNodes() const
{
	return NodeMap;
}

void FTerrainNodeFactory::Clear()
{
	NodeMap.Empty();
}