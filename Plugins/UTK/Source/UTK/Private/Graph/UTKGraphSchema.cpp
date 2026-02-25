#include "Graph/UTKGraphSchema.h"

#include "Assets/UTKAsset.h"
#include "Core/UTKLogger.h"
#include "Framework/Commands/GenericCommands.h"
#include "Graph/UTKGraph.h"
#include "Graph/Nodes/UTKNode.h"
#include "Graph/Nodes/UTKNodeFactory.h"

#include "Editor.h"
#include "Editor/UnrealEdEngine.h"
#include "UnrealEdGlobals.h"

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
	if (A->Direction == B->Direction)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Pins must be compatible and opposite."));
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}

void UUTKGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	// Optional: Add default nodes to the graph
}

UEdGraphNode* FUTKSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2f& Location, bool bSelectNewNode)
{
	if (!ParentGraph)
		return nullptr;

	FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "AddUTKNode", "Add Node"));

	UUTKNode* NewNode = NewObject<UUTKNode>(ParentGraph, UUTKNode::StaticClass(), NAME_None, RF_Transactional);

	NewNode->NodeType = FName(*NodeDef.Name);
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
bool UUTKGraphSchema::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	if (A->Direction == EGPD_Input && B->Direction == EGPD_Output)
	{
		Swap(A, B);
	}

	if (A->Direction != EGPD_Output || B->Direction != EGPD_Input)
	{
		return false;
	}

	for (int32 i = B->LinkedTo.Num() - 1; i >= 0; --i)
	{
		UEdGraphPin* OldOutput = B->LinkedTo[i];
		if (OldOutput)
		{
			OldOutput->BreakLinkTo(B);
		}
	}

	A->MakeLinkTo(B);

	if (UEdGraph* Graph = A->GetOwningNode()->GetGraph())
	{
		Graph->Modify();
		Graph->NotifyGraphChanged();
	}

	return true;
}