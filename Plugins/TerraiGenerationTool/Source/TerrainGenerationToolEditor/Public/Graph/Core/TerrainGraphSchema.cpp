#include "Graph/Core/TerrainGraphSchema.h"
#include "Graph/Core/TerrainGraphNode.h"
#include "Graph/Core/TerrainNodeRegistry.h"

void UTerrainGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	if (!ContextMenuBuilder.CurrentGraph) return;

	for (const auto& Entry : FTerrainNodeRegistry::Get())
	{
		ContextMenuBuilder.AddAction(MakeShared<FEdGraphSchemaAction_NewNode>(
			FText::FromString(Entry.Cetegory),
			FText::FromString(Entry.DisplayName),
			FText::GetEmpty(),
			0,
			nullptr,
			[Entry](UEdGraph* Graph, UEdGraphPin* /*FromPin*/){
				FGraphNodeCreator<UTerrainGraphNode> Creator(Graph, Entry.NodeClass);
				auto* Node = Creator.CreateNode();
				Creator.Finalize();
				Node->AlloacteDefaultPins();
			}
		));
	}
}

const FPinConnectionResponse UTerrainGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}
