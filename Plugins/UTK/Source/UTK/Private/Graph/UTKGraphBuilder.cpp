#include "Graph/UTKGraphBuilder.h"

#include "Assets/UTKAsset.h"
#include "Core/UTKLogger.h"
#include "Graph/UTKGraph.h"
#include "Graph/Nodes/UTKNode.h"
#include "Graph/Nodes/UTKNodeFactory.h"

void FUTKGraphBuilder::LoadFromAsset(UUTKAsset* Asset, UUTKGraph* Graph)
{
	if (!Asset || !Graph) return;

	Graph->Nodes.Empty();

	TMap<FGuid, UUTKNode*> NodeMap;

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
		Node->NodeGuid = NodeData->Guid;
		NodeMap.Add(Node->NodeGuid, Node);
	}

	for (const UUTKGraphNodeSaveData* NodeData : Asset->SavedGraph->Nodes)
	{
		UUTKNode* FromNode = NodeMap.FindRef(NodeData->Guid);
		if (!FromNode) continue;

		for (const UUTKGraphPinSaveData* PinData : NodeData->Pins)
		{
			if (!PinData || PinData->bInput) continue;

			for (UEdGraphPin* FromPin : FromNode->Pins)
			{
				if (FromPin->PinName != PinData->Name || FromPin->Direction != EGPD_Output)
					continue;

				for (int32 i = 0; i < PinData->ConnectedToNodes.Num(); ++i)
				{
					const FGuid& ToNodeGuid = PinData->ConnectedToNodes[i];
					const FName& ToPinName = PinData->ConnectedPins.IsValidIndex(i) ? PinData->ConnectedPins[i] : NAME_None;

					UUTKNode* ToNode = NodeMap.FindRef(ToNodeGuid);
					if (!ToNode) continue;

					for (UEdGraphPin* ToPin : ToNode->Pins)
					{
						if (ToPin->PinName == ToPinName && ToPin->Direction == EGPD_Input)
						{
							FromPin->MakeLinkTo(ToPin);
							break;
						}
					}
				}
			}
		}
	}
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
			NodeData->Guid = Node->NodeGuid;
			NodeData->NodeType = Node->GetDefinition().Name;
			NodeData->Position = FVector2D(Node->NodePosX, Node->NodePosY);

			for (UEdGraphPin* Pin : Node->Pins)
			{

				UUTKGraphPinSaveData* PinData = NewObject<UUTKGraphPinSaveData>(NodeData);
				PinData->Name = Pin->PinName;
				PinData->bInput = Pin->Direction == EGPD_Input;

				if (Pin->Direction == EGPD_Output)
				{
					for (UEdGraphPin* Linked : Pin->LinkedTo)
					{
						if (UUTKNode* LinkedNode = Cast<UUTKNode>(Linked->GetOwningNode()))
						{
							PinData->ConnectedToNodes.Add(LinkedNode->NodeGuid);
							PinData->ConnectedPins.Add(Linked->PinName);
						}
					}
				}


				NodeData->Pins.Add(PinData);
			}

			SaveData->Nodes.Add(NodeData);
		}
	}
}