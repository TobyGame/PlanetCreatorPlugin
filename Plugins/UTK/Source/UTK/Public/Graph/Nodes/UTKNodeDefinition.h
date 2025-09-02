#pragma once

#include "StructUtils/InstancedStruct.h"
#include "Core/UTKDataSystem.h"


struct FUTKNodePinDefinition
{
	FName Name;
	bool bInput;
	bool bRequired;
};

struct FUTKNodePropertyDefinition
{
	FName Name;
	TFunction<FInstancedStruct()> Value;
	FName Category;
};

struct FUTKNodeDefinition
{
	FString Name;
	FString DisplayName;
	FString Category;
	TArray<FUTKNodePinDefinition> Pins;
	TArray<FUTKNodePropertyDefinition> Properties;
	TFunction<void(FUTKDataSet& DataSet, class UUTKNode* Node, int32 ResolutionX, int32 ResolutionY)> ProcessFunction;
};