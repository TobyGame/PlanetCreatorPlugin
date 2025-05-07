#pragma once

#include "CoreMinimal.h"
#include "TerrainNodeDefinition.h"

class FTerrainNodeFactory
{
public:
	static FTerrainNodeFactory& Get();

	void RegisterNode(const FTerrainNodeDefinition& Definition);
	const TMap<FString, FTerrainNodeDefinition>& GetAllNodes() const;
	void Clear();

private:
	TMap<FString, FTerrainNodeDefinition> NodeMap;
};