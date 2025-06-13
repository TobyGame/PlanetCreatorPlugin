#include "Graph/UTKGraphBuilder.h"

#include "Assets/UTKAsset.h"
#include "Core/UTKLoggerEditor.h"
#include "Graph/UTKGraph.h"
#include "Graph/Nodes/UTKNode.h"
#include "Graph/Nodes/UTKNodeFactory.h"

void FUTKGraphBuilder::LoadFromAsset(UUTKAsset* Asset, UUTKGraph* Graph)
{
	if (!Asset || !Graph) return;

	Graph->Nodes.Empty();

	UE_LOG(LogUTKEditor,
		Log,
		TEXT("Loading %d nodes from asset '%s'."),
		Asset->SavedGraph->Nodes.Num(),
		*Asset->GetName());

	for (const UUTKGraphNodeSaveData* NodeData : Asset->SavedGraph->Nodes)
	{
		const FUTKNodeDefinition* Def = FUTKNodeFactory::Get().Find(NodeData->NodeType);
		if (!Def) continue;

		UUTKNode* Node = NewObject<UUTKNode>(Graph);
		Node->SetDefinition(*Def);
		Node->NodeGuid = FGuid::NewGuid();
		Node->NodePosX = NodeData->Position.X;
		Node->NodePosY = NodeData->Position.Y;
		Node->AllocateDefaultPins();

		Graph->AddNode(Node);
	}

	// TODO: Wire up pin connections
}

void FUTKGraphBuilder::SaveToAsset(UUTKGraph* Graph, UUTKAsset* Asset)
{
	if (!Graph || !Asset)
		return;

	UE_LOG(LogUTKEditor,
		Log,
		TEXT("Saving %d nodes from graph to asset '%s'."),
		Graph->Nodes.Num(),
		*Asset->GetName());

	UUTKGraphSaveData* SaveData = Asset->SavedGraph;
	SaveData->Modify();
	SaveData->Nodes.Empty();

	for (UEdGraphNode* GraphNode : Graph->Nodes)
	{
		if (UUTKNode* Node = Cast<UUTKNode>(GraphNode))
		{
			UUTKGraphNodeSaveData* NodeData = NewObject<UUTKGraphNodeSaveData>(SaveData);
			NodeData->Id = Node->GetFName();
			NodeData->NodeType = Node->GetDefinition().Name;
			NodeData->Position = FVector2D(Node->NodePosX, Node->NodePosY);
			SaveData->Nodes.Add(NodeData);
		}
	}
}