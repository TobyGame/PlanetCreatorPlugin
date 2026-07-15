#pragma once

#include "UTKOperatorDefinition.h"

struct FUTKNodeDefinition;

class FUTKOperatorRegistry
{
public:
	static FUTKOperatorRegistry& Get();

	bool RegisterOperator(const FUTKOperatorDefinition& Definition);

	const FUTKOperatorDefinition* FindOperator(FName OperatorId) const;

	bool ValidateNodeDefinition(const FUTKNodeDefinition& NodeDefinition, FString& OutError) const;

	void Clear();

private:
	TMap<FName, FUTKOperatorDefinition> Operators;
};