#include "Graph/TerrainGraphBuilder.h"

#include "Assets/TerrainAsset.h"
#include "Graph/TerrainGraph.h"
#include "Graph/Nodes/TerrainNode.h"
#include "Graph/Nodes/TerrainNodeFactory.h"
#include "Core/TerrainData.h"

void FTerrainGraphBuilder::LoadFromAsset(UTerrainAsset* Asset, UTerrainGraph* Graph)
{
	if (!Asset || !Graph) return;

	Graph->Nodes.Empty();

	for (const UTerrainGraphNodeSaveData* NodeData : Asset->SavedGraph->Nodes)
	{
		const FTerrainNodeDefinition* Def = FTerrainNodeFactory::Get().Find(NodeData->NodeType);
		if (!Def) continue;

		UTerrainNode* Node = NewObject<UTerrainNode>(Graph);
		Node->SetDefinition(*Def);
		Node->NodeGuid = FGuid::NewGuid();
		Node->NodePosX = NodeData->Position.X;
		Node->NodePosY = NodeData->Position.Y;
		Node->AllocateDefaultPins();

		Graph->AddNode(Node);
	}

	// TODO: Wire up pin connections
}

void FTerrainGraphBuilder::SaveToAsset(UTerrainGraph* Graph, UTerrainAsset* Asset)
{
	if (!Graph || !Asset)
		return;

	UTerrainGraphSaveData* SaveData = Asset->SavedGraph;
	SaveData->Modify();
	SaveData->Nodes.Empty();

	for (UEdGraphNode* GraphNode : Graph->Nodes)
	{
		if (UTerrainNode* Node = Cast<UTerrainNode>(GraphNode))
		{
			UTerrainGraphNodeSaveData* NodeData = NewObject<UTerrainGraphNodeSaveData>(SaveData);
			NodeData->Id = Node->GetFName();
			NodeData->NodeType = Node->GetDefinition().Name;
			NodeData->Position = FVector2D(Node->NodePosX, Node->NodePosY);
			SaveData->Nodes.Add(NodeData);
		}
	}
}