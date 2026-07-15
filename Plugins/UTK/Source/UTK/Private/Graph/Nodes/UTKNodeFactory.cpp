#include "Graph/Nodes/UTKNodeFactory.h"

#include "Core/UTKLogger.h"
#include "Graph/Operators/UTKOperatorRegistry.h"

FUTKNodeFactory& FUTKNodeFactory::Get()
{
	static FUTKNodeFactory Instance;
	return Instance;
}

bool FUTKNodeFactory::RegisterNode(const FUTKNodeDefinition& Definition)
{
	FString Error;

	if (!Definition.IsValid(&Error))
	{
		UE_LOG(LogUTKEditor, Error, TEXT("[UTK] Refused invalid node registration: %s"), *Error);
		return false;
	}

	if (!FUTKOperatorRegistry::Get().ValidateNodeDefinition(Definition, Error))
	{
		UE_LOG(LogUTKEditor, Error, TEXT("[UTK] Refused node '%s': %s"), *Definition.TypeId.ToString(), *Error);
		return false;
	}

	if (NodeMap.Contains(Definition.TypeId))
	{
		UE_LOG(LogUTKEditor, Error, TEXT("[UTK] Duplicate node registration: %s"), *Definition.TypeId.ToString());
		return false;
	}

	NodeMap.Add(Definition.TypeId, Definition);
	return true;
}

const TMap<FName, FUTKNodeDefinition>& FUTKNodeFactory::GetAllNodes() const
{
	return NodeMap;
}

void FUTKNodeFactory::Clear()
{
	NodeMap.Empty();
}

const FUTKNodeDefinition* FUTKNodeFactory::Find(FName TypeId) const
{
	return NodeMap.Find(TypeId);
}

bool FUTKNodeFactory::GetDefinition(FName TypeId, FUTKNodeDefinition& OutDefinition) const
{
	if (const FUTKNodeDefinition* Found = NodeMap.Find(TypeId))
	{
		OutDefinition = *Found;
		return true;
	}
	return false;
}