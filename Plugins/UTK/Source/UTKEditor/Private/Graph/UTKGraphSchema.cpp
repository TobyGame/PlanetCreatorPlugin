#include "Graph/UTKGraphSchema.h"

#include "Core/UTKLogger.h"
#include "Core/UTKLoggerEditor.h"
#include "Framework/Commands/GenericCommands.h"
#include "Graph/Nodes/UTKNode.h"
#include "Graph/Nodes/UTKNodeFactory.h"

void UUTKGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	const auto& Nodes = FUTKNodeFactory::Get().GetAllNodes();

	for (const auto& NodePair : Nodes)
	{
		const auto& Def = NodePair.Value;
		TSharedPtr<FUTKSchemaAction_NewNode> Action = MakeShareable(new FUTKSchemaAction_NewNode(Def));
		ContextMenuBuilder.AddAction(Action);
	}
}

const FPinConnectionResponse UUTKGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	if (A->Direction == B->Direction || A->PinType != B->PinType)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Pins must be compatible and opposite."));
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}

void UUTKGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	// Optional: Add default nodes to the graph
}

UEdGraphNode* FUTKSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	if (!ParentGraph)
		return nullptr;

	FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "AddUTKNode", "Add Node"));

	UUTKNode* NewNode = NewObject<UUTKNode>(ParentGraph);
	NewNode->SetDefinition(NodeDef);
	NewNode->NodePosX = Location.X;
	NewNode->NodePosY = Location.Y;
	NewNode->AllocateDefaultPins();

	NewNode->Modify();
	ParentGraph->Modify();
	ParentGraph->AddNode(NewNode, true, true);

	return NewNode;
}

void UUTKGraphSchema::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	Super::GetContextMenuActions(Menu, Context);

	UE_LOG(LogUTKEditor, Verbose, TEXT("Building context menu for graph schema."));

	if (!Context->bIsDebugging)
	{
		FToolMenuSection& Section = Menu->FindOrAddSection("UTKGraph");

		Section.AddMenuEntry(FGenericCommands::Get().Delete);
	}
}