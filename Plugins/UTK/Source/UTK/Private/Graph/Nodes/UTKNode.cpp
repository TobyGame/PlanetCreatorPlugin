#include "Graph/Nodes/UTKNode.h"

#include "Graph/Nodes/UTKNodeFactory.h"

UUTKNode::UUTKNode()
{
	NodeGuid = FGuid::NewGuid();
}

void UUTKNode::PostLoad()
{
	Super::PostLoad();
	RebuildDefinitionFromType();
}

void UUTKNode::PostDuplicate(bool bDuplicateForPIE)
{
	Super::PostDuplicate(bDuplicateForPIE);
	RebuildDefinitionFromType();
}

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
	NodeType = FName(*InDefinition.Name);
	EnsureSettingsInstance(InDefinition);
}

const FUTKNodeDefinition& UUTKNode::GetDefinition() const
{
	return NodeDefinition;
}

void UUTKNode::EnsureSettingsInstance(const FUTKNodeDefinition& Definition)
{
	if (Settings)
		return;

	if (Definition.SettingsClass &&
		Definition.SettingsClass->IsChildOf(UUTKNodeSettings::StaticClass()))
	{
		Settings = NewObject<UUTKNodeSettings>(
			this,
			Definition.SettingsClass,
			NAME_None,
			RF_Transactional
		);
	}
}

void UUTKNode::RebuildDefinitionFromType()
{
	if (NodeType.IsNone())
		return;

	FUTKNodeDefinition Definition;
	if (FUTKNodeFactory::Get().GetDefinition(NodeType, Definition))
		SetDefinition(Definition);
}