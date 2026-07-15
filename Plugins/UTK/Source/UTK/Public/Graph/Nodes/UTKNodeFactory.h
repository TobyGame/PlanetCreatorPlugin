#pragma once

#include "Graph/Nodes/UTKNodeDefinition.h"

class FUTKNodeFactory
{
public:
	static FUTKNodeFactory& Get();

	bool RegisterNode(const FUTKNodeDefinition& Definition);
	const TMap<FName, FUTKNodeDefinition>& GetAllNodes() const;
	void Clear();
	const FUTKNodeDefinition* Find(FName TypeId) const;

	bool GetDefinition(FName TypeId, FUTKNodeDefinition& OutDefinition) const;

private:
	TMap<FName, FUTKNodeDefinition> NodeMap;
};