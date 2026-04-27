#include "Graph/UTKGraphSchema.h"
#include "UI/Toolkit/UTKEditorApp.h"
#include "UI/Graph/UTKGraphCommands.h"
#include "Graph/Nodes/UTKNode.h"
#include "Graph/Nodes/UTKNodeFactory.h"
#include "Editor.h"

#define LOCTEXT_NAMESPACE "UTKGraphSchema"

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

	if (!Menu || !Context)
		return;

	//-------------------------
	// Pin context menu actions
	//-------------------------
	if (Context->Pin)
	{
		const UEdGraphPin* ContextPin = Context->Pin;
		const UEdGraphNode* ContextOwnerNode = ContextPin->GetOwningNode();

		TWeakObjectPtr<UEdGraphNode> WeakOwnerNode = const_cast<UEdGraphNode*>(ContextOwnerNode);
		const FName PinName = ContextPin->PinName;
		const EEdGraphPinDirection PinDirection = ContextPin->Direction;

		FToolMenuSection& PinSection = Menu->AddSection(
			"UTKPinActions",
			LOCTEXT("UTKPinActions", "Pin")
		);

		PinSection.AddMenuEntry(
			"UTK_BreakPinLinks",
			LOCTEXT("UTKBreakPinLinks", "Break Pin Links"),
			LOCTEXT("UTKBreakPinLinks_Tooltip", "Break all links on this pin."),
			FSlateIcon(),
			FToolUIActionChoice(
				FExecuteAction::CreateLambda([WeakOwnerNode, PinName, PinDirection](){
					UEdGraphNode* OwnerNode = WeakOwnerNode.Get();
					if (!OwnerNode)
						return;

					UEdGraphPin* Pin = OwnerNode->FindPin(PinName, PinDirection);
					if (!Pin)
						return;

					const FScopedTransaction Transaction(LOCTEXT("UTKBreakPinLinks_Transaction", "Break Pin Links"));

					OwnerNode->Modify();
					Pin->Modify();
					Pin->BreakAllPinLinks();
				})
			)
		);
	}

	//--------------------------
	// Node context menu actions
	//--------------------------
	if (Context->Node)
	{
		const UEdGraphNode* ContextNode = Context->Node;
		TWeakObjectPtr<UEdGraphNode> WeakNode = const_cast<UEdGraphNode*>(ContextNode);

		FToolMenuSection& NodeSection = Menu->AddSection(
			"UTKNodeActions",
			LOCTEXT("UTKNodeActions", "Node")
		);

		TSharedPtr<FUTKEditorApp> Editor = FUTKEditorApp::GetLastInstance();
		if (Editor.IsValid())
		{
			TWeakObjectPtr<UEdGraphNode> WeakContextNode = const_cast<UEdGraphNode*>(ContextNode);
			TWeakPtr<FUTKEditorApp> WeakEditor = Editor;

			TSharedRef<FUICommandList> ContextCommandList = MakeShared<FUICommandList>();

			ContextCommandList->MapAction(
				FUTKGraphCommands::Get().LockPreview,
				FExecuteAction::CreateLambda([WeakEditor, WeakContextNode](){
					TSharedPtr<FUTKEditorApp> PinnedEditor = WeakEditor.Pin();
					if (!PinnedEditor.IsValid())
						return;

					UEdGraphNode* GraphNode = WeakContextNode.Get();
					if (!GraphNode)
						return;

					UUTKNode* UTKNode = Cast<UUTKNode>(GraphNode);
					if (!UTKNode)
						return;

					PinnedEditor->TogglePreviewLockForNode(UTKNode);
				}),
				FCanExecuteAction::CreateLambda([WeakContextNode](){
					return WeakContextNode.IsValid() && Cast<UUTKNode>(WeakContextNode.Get()) != nullptr;
				})
			);

			NodeSection.AddMenuEntryWithCommandList(
				FUTKGraphCommands::Get().LockPreview,
				Editor->GetToolkitCommands()
			);
		}

		NodeSection.AddMenuEntry(
			"UTK_BreakNodeLinks",
			LOCTEXT("UTKBreakNodeLinks", "Break Node Links"),
			LOCTEXT("UTKBreakNodeLinks_Tooltip", "Break all links connected to this node."),
			FSlateIcon(),
			FToolUIActionChoice(
				FExecuteAction::CreateLambda([WeakNode](){
					UEdGraphNode* Node = WeakNode.Get();
					if (!Node)
						return;

					const FScopedTransaction Transaction(LOCTEXT("UTKBreakNodeLinks_Transaction", "Break Node Links"));

					Node->Modify();
					Node->BreakAllNodeLinks();
				})
			)
		);

		NodeSection.AddMenuEntry(
			"UTK_DeleteNode",
			LOCTEXT("UTKDeleteNode", "Delete Node"),
			LOCTEXT("UTKDeleteNode_Tooltip", "Delete this node from the graph."),
			FSlateIcon(),
			FToolUIActionChoice(
				FExecuteAction::CreateLambda([WeakNode](){
					UEdGraphNode* Node = WeakNode.Get();
					if (!Node)
						return;

					UEdGraph* Graph = Node->GetGraph();
					if (!Graph)
						return;

					const FScopedTransaction Transaction(LOCTEXT("UTKDeleteNode_Transaction", "Delete Node"));

					Graph->Modify();
					Node->Modify();
					Node->DestroyNode();
				})
			)
		);
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

#undef LOCTEXT_NAMESPACE