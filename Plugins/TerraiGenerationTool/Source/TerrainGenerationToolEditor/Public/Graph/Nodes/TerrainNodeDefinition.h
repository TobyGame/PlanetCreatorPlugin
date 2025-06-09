#pragma once


#include "StructUtils/InstancedStruct.h"
#include "Core/TerrainDataSystem.h"

struct FTerrainData;

struct FTerrainNodePinDefinition
{
	FName Name;
	bool bInput;
	bool bRequired;
};

struct FTerrainNodePropertyDefinition
{
	FName Name;
	TFunction<FInstancedStruct()> Value;
	FName Category;
};

struct FTerrainNodeDefinition
{
	FString Name;
	FString DisplayName;
	FString Category;
	TArray<FTerrainNodePinDefinition> Pins;
	TArray<FTerrainNodePropertyDefinition> Properties;
	TFunction<void(FTerrainDataSet& DataSet, class UTerrainNode* Node, int32 ResolutionX, int32 ResolutionY)> ProcessFunction;
};