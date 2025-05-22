#include "Graph/TerrainGraphSchema.h"
#include "Graph/Nodes/TerrainNode.h"
#include "Graph/Nodes/TerrainNodeFactory.h"

void UTerrainGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	const auto& Nodes = FTerrainNodeFactory::Get().GetAllNodes();

	for (const auto& NodePair : Nodes)
	{
		const auto& Def = NodePair.Value;
		TSharedPtr<FTerrainSchemaAction_NewNode> Action = MakeShareable(new FTerrainSchemaAction_NewNode(Def));
		ContextMenuBuilder.AddAction(Action);
	}
}

const FPinConnectionResponse UTerrainGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	if (A->Direction == B->Direction || A->PinType != B->PinType)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Pins must be compatible and opposite."));
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}

void UTerrainGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	// Optional: Add default nodes to the graph
}

UEdGraphNode* FTerrainSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	if (!ParentGraph)
		return nullptr;

	UTerrainNode* NewNode = NewObject<UTerrainNode>(ParentGraph);
	NewNode->SetDefinition(NodeDef);
	NewNode->NodePosX = Location.X;
	NewNode->NodePosY = Location.Y;
	NewNode->AllocateDefaultPins();

	ParentGraph->Modify();
	ParentGraph->AddNode(NewNode, true, true);

	return NewNode;
}