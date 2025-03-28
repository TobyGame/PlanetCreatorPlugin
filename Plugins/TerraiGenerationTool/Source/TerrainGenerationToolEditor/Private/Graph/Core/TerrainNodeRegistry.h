#pragma once

#include "CoreMinimal.h"

class UTerrainGraphNode;

struct FTerrainNodeRegistryEntry
{
	FString Category;
	FString DisplayName;
	TSubclassOf<UTerrainGraphNode> NodeClass;
};

class FTerrainNodeRegistry
{
public:
	static void Register(const FString& Category, const FString& DisplayName, TSubclassOf<UTerrainGraphNode> NodeClass);
	static const TArray<FTerrainNodeRegistryEntry>& Get();
};