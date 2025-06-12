#include "Graph/Nodes/UTKNode.h"

UUTKNode::UUTKNode() {}

void UUTKNode::AllocateDefaultPins()
{
	for (const auto& PinDef : NodeDefinition.Pins)
	{
		const EEdGraphPinDirection Direction = PinDef.bInput ? EGPD_Input : EGPD_Output;
		CreatePin(Direction, NAME_None, PinDef.Name);
	}
}

FText UUTKNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(NodeDefinition.DisplayName);
}

void UUTKNode::SetDefinition(const FUTKNodeDefinition& InDefinition)
{
	NodeDefinition = InDefinition;

	for (const auto& Property : NodeDefinition.Properties)
	{
		RuntimeProperties.Add(Property.Name, Property.Value());
	}
}

const FUTKNodeDefinition& UUTKNode::GetDefinition() const
{
	return NodeDefinition;
}