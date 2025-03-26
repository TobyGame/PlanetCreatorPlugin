#pragma once

#include "Graph/Core/TerrainGraphNode.h"
#include "Graph/Core/TerrainNodeRegistry.h"

#ifndef TERRAIN_NODE_CATEGORY
#define TERRAIN_NODE_CATEGORY "Uncategorized"
#endif


#define BEGIN_TERRAIN_NODE(NodeClass, DisplayNameStr) \
	class NodeClass : public FTerrainGraphNode \
	{ \
		GENERATED_BODY() \
	public: \
		NodeClass() \
		{ \
			NodeDisplayName = FText::FromString(DisplayNameStr); \
			CategoryName = TEXT(TERRAIN_NODE_CATEGORY); \
		} \
		virtual void DefinePins(TArray<FTerrainPinDescriptor>& OutPins) override;

#define TERRAIN_INPUT(Index, PinNameLiteral, ValueType, Optional) \
	OutPins.Add({ FName(PinNameLiteral), ValueType, Index, true, Optional, true });

#define TERRAIN_OUTPUT(Index, PinNameLiteral, ValueType) \
	OutPins.Add({ FName(PinNameLiteral), ValueType, Index, false, false, true });

#define TERRAIN_OPTIONAL_INPUT(Index, PinNameLiteral, ValueType) \
	OutPins.Add({ FName(PinNameLiteral), ValueType, Index, true, true, true });

#define TERRAIN_OPTIONAL_OUTPUT(Index, PinNameLiteral, ValueType) \
	OutPins.Add({ FName(PinNameLiteral), ValueType, Index, false, true, true });

#define TERRAIN_PROPERTY(Type, Name, DefaultValue, PropertyCategory) \
	UPROPERTY(EditAnywhere, Category = PropertyCategory) \
	Type Name = DefaultValue;

#define END_TERRAIN_NODE() \
	}; \

#define REGISTER_TERRAIN_NODE(NodeClass) \
	namespace { struct FAutoRegister_##NodeClass \
	{ \
		FAutoRegister_##NodeClass() \
		{ \
			FTerrainNodeRegistry::Register( \
				TEXT(TERRAIN_NODE_CATEGORY), \
				NodeClass().GetNodeTitle(ENodeTitleType::ListView).ToString(), \
				NodeClass::StaticClass()); \
		} \
	}; static FAutoRegister_##NodeClass AutoRegister_##NodeClass_Instance; }