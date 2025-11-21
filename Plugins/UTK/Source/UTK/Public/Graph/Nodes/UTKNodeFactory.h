#pragma once

#include "Graph/Nodes/UTKNodeDefinition.h"

class FUTKNodeFactory
{
public:
	static FUTKNodeFactory& Get();

	void RegisterNode(const FUTKNodeDefinition& Definition);
	const TMap<FString, FUTKNodeDefinition>& GetAllNodes() const;
	void Clear();
	const FUTKNodeDefinition* Find(const FString& Name) const;

	bool GetDefinition(const FName& Type, FUTKNodeDefinition& OutDefinition) const;

private:
	TMap<FString, FUTKNodeDefinition> NodeMap;
};