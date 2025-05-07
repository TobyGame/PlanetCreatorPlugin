#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "TerrainNodeDefinition.h"
#include "TerrainNode.generated.h"

UCLASS()
class UTerrainNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
	UTerrainNode();

	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	void SetDefinition(const FTerrainNodeDefinition& InDefinition);
	const FTerrainNodeDefinition& GetDefinition() const;

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
	FTerrainNodeDefinition NodeDefinition;

	TMap<FName, FInstancedStruct> RuntimeProperties;
};