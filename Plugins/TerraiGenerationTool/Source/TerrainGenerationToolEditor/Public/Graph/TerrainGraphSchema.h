#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"
#include "Nodes/TerrainNodeDefinition.h"
#include "TerrainGraphSchema.generated.h"

UCLASS()
class UTerrainGraphSchema : public UEdGraphSchema
{
	GENERATED_BODY()

public:
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
	virtual void GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;
};


USTRUCT()
struct FTerrainSchemaAction_NewNode : public FEdGraphSchemaAction
{
	GENERATED_BODY()

	FTerrainNodeDefinition NodeDef;

	FTerrainSchemaAction_NewNode() {}
	FTerrainSchemaAction_NewNode(const FTerrainNodeDefinition& InDefinition)
		: FEdGraphSchemaAction(
			  FText::FromString(InDefinition.Category),
			  FText::FromString(InDefinition.Name),
			  FText::FromString("Adds a new terrain node"),
			  0),
		  NodeDef(InDefinition)
	{}

	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
};