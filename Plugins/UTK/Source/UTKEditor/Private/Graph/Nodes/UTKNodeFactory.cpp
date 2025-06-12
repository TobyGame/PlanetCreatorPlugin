#include "Graph/Nodes/UTKNodeFactory.h"

FUTKNodeFactory& FUTKNodeFactory::Get()
{
	static FUTKNodeFactory Instance;
	return Instance;
}

void FUTKNodeFactory::RegisterNode(const FUTKNodeDefinition& Definition)
{
	NodeMap.Add(Definition.Name, Definition);
}

const TMap<FString, FUTKNodeDefinition>& FUTKNodeFactory::GetAllNodes() const
{
	return NodeMap;
}

void FUTKNodeFactory::Clear()
{
	NodeMap.Empty();
}

const FUTKNodeDefinition* FUTKNodeFactory::Find(const FString& Name) const
{
	if (const FUTKNodeDefinition* Found = NodeMap.Find(Name))
	{
		return Found;
	}
	return nullptr;
}