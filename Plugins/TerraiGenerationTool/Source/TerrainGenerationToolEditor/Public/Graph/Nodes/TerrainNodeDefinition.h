#pragma once

#include "CoreMinimal.h"
#include "Core/TerrainData.h"
#include "StructUtils/InstancedStruct.h"
#include "TerrainDataType.h"
#include "NodeProperties/TerrainPropertyTypes.h"

struct FTerrainNodePinDefinition
{
	FName Name;
	ETerrainDataType Type;
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
	TFunction<TSharedPtr<FTerrainData>(const TArray<TSharedPtr<FTerrainData>>&, class UTerrainNode*)> ProcessFunction;
};