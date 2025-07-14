#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"
#include "Nodes/UTKNodeDefinition.h"
#include "UTKGraphSchema.generated.h"

UCLASS()
class UUTKGraphSchema : public UEdGraphSchema
{
	GENERATED_BODY()

public:
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
	virtual void GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;
	virtual bool TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const override;
};


USTRUCT()
struct FUTKSchemaAction_NewNode : public FEdGraphSchemaAction
{
	GENERATED_BODY()

	FUTKNodeDefinition NodeDef;

	FUTKSchemaAction_NewNode() {}
	FUTKSchemaAction_NewNode(const FUTKNodeDefinition& InDefinition)
		: FEdGraphSchemaAction(
			  FText::FromString(InDefinition.Category),
			  FText::FromString(InDefinition.Name),
			  FText::FromString("Adds a new terrain node"),
			  0),
		  NodeDef(InDefinition)
	{}

	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
};