﻿#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"
#include "TerrainGraphSchema.generated.h"

UCLASS()
class UTerrainGraphSchema : public UEdGraphSchema
{
	GENERATED_BODY()

public:
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
};