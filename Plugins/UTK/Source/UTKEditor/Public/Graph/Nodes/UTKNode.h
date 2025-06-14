﻿#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "UTKNodeDefinition.h"
#include "UTKNode.generated.h"

UCLASS()
class UUTKNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
	UUTKNode();

	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	void SetDefinition(const FUTKNodeDefinition& InDefinition);
	const FUTKNodeDefinition& GetDefinition() const;

	template <typename T>
	T GetProperty(const FName& PropertyName, const T& Default = T()) const
	{
		const FInstancedStruct* Found = RuntimeProperties.Find(PropertyName);
		if (!Found || !Found->IsValid())
		{
			return Default;
		}

		const T* ValuePtr = Found->GetPtr<T>();

		if (ValuePtr)
		{
			return *ValuePtr;
		}

		return Default;
	}

private:
	FUTKNodeDefinition NodeDefinition;

	TMap<FName, FInstancedStruct> RuntimeProperties;
};