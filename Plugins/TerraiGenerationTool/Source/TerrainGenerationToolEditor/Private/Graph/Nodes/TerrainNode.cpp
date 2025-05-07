#include "Graph/Nodes/TerrainNode.h"

UTerrainNode::UTerrainNode() {}

void UTerrainNode::AllocateDefaultPins()
{
	for (const auto& PinDef : NodeDefinition.Pins)
	{
		const EEdGraphPinDirection Direction = PinDef.bInput ? EGPD_Input : EGPD_Output;
		CreatePin(Direction, FName(*UEnum::GetValueAsString(PinDef.Type)), PinDef.Name);
	}
}

FText UTerrainNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(NodeDefinition.DisplayName);
}

void UTerrainNode::SetDefinition(const FTerrainNodeDefinition& InDefinition)
{
	NodeDefinition = InDefinition;

	for (const auto& Property : NodeDefinition.Properties)
	{
		RuntimeProperties.Add(Property.Name, Property.Value());
	}
}

const FTerrainNodeDefinition& UTerrainNode::GetDefinition() const
{
	return NodeDefinition;
}