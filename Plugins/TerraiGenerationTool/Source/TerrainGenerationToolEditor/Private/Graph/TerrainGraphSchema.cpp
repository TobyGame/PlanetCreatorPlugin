#include "Graph/TerrainGraphSchema.h"
#include "Graph/Nodes/TerrainNodeCore.h"



void UTerrainGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	const auto& Nodes = FTerrainNodeFactory::Get().GetAllNodes();

	for (const auto& NodePair : Nodes)
	{
		const auto& Def = NodePair.Value;
		TSharedPtr<FEdGraphSchemaAction> Action = MakeShared<FEdGraphSchemaAction_NewNode>(
			FText::FromString(Def.Category),
			FText::FromString(Def.DisplayName),
			FText::GetEmpty(),
			0
		);

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