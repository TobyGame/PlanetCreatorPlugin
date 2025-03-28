#include "Graph/Core/TerrainGraphNode.h"

UTerrainGraphNode::UTerrainGraphNode()
{
	NodeDisplayName = FText::FromString("Unnamed");
	CategoryName = TEXT("Default");
}

FText UTerrainGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return NodeDisplayName;
}

FString UTerrainGraphNode::GetCategoryName() const
{
	return CategoryName;
}

void UTerrainGraphNode::AllocateDefaultPins()
{
	PinDescriptions.Reset();
	DefinePins(PinDescriptions);

	for (const FTerrainPinDescriptor& Desc : PinDescriptions)
	{
		FName PinType;
		switch (Desc.DataType)
		{
		case ETerrainValueType::Float:		PinType = "Float"; break;
		case ETerrainValueType::HeightMap:	PinType = "HeightMap"; break;
		case ETerrainValueType::Mask:		PinType = "Mask"; break;
		case ETerrainValueType::Vector2D:	PinType = "Vector2D"; break;
		default:							PinType = "Unknown"; break;
		}

		CreatePin(
			Desc.bIsInput ? EGPD_Input : EGPD_Output,
			PinType,
			Desc.PinName
		);
	}
}