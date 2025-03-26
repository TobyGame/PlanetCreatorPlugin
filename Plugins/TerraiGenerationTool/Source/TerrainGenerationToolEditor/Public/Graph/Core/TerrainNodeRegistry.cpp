#include "Graph/Core/TerrainNodeRegistry.h"

static TArray<FTerrainNodeRegistryEntry> Registry;

void FTerrainNodeRegistry::Register(const FString& Category, const FString& DisplayName, TSubclassOf<UTerrainGraphNode> NodeClass)
{
	Registry.Add({Category, DisplayName, NodeClass});
}

const TArray<FTerrainNodeRegistryEntry>& FTerrainNodeRegistry::Get()
{
	return Registry;
}
