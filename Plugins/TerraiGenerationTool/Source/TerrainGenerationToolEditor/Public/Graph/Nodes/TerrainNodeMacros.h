#pragma once

#include "TerrainNodeDefinition.h"
#include "TerrainNodeFactory.h"

#define DEFINE_PIN(Name, bInput, bRequired) FTerrainNodePinDefinition{ FName(Name), bInput, bRequired }
#define DEFINE_PROPERTY(Name, Default, Category) FTerrainNodePropertyDefinition{ FName(Name), []() { return FInstancedStruct::Make<decltype(Default)>(Default); }, FName(Category) }

#define DECLARE_TERRAIN_NODE(Name, DisplayName, Category, Pins, Properties, Logic) \
	FTerrainNodeDefinition Create_##Name##_NodeDefinition() \
	{ \
		return FTerrainNodeDefinition{ \
			#Name, DisplayName, Category, Pins, Properties, Logic \
		}; \
	}

#define REGISTER_TERRAIN_NODE(Name) \
	FTerrainNodeFactory::Get().RegisterNode(Create_##Name##_NodeDefinition());