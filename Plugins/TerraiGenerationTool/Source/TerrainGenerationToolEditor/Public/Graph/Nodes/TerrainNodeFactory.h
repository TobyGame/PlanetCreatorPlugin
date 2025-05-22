#pragma once

#include "Graph/Nodes/TerrainNodeDefinition.h"

class FTerrainNodeFactory
{
public:
	static FTerrainNodeFactory& Get();

	void RegisterNode(const FTerrainNodeDefinition& Definition);
	const TMap<FString, FTerrainNodeDefinition>& GetAllNodes() const;
	void Clear();
	const FTerrainNodeDefinition* Find(const FString& Name) const;

private:
	TMap<FString, FTerrainNodeDefinition> NodeMap;
};