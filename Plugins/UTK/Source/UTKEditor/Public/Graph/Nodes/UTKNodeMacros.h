#pragma once

#include "UTKNodeDefinition.h"
#include "UTKNodeFactory.h"

#define DEFINE_PIN(Name, bInput, bRequired) FUTKNodePinDefinition{ FName(Name), bInput, bRequired }
#define DEFINE_PROPERTY(Name, Default, Category) FUTKNodePropertyDefinition{ FName(Name), []() { return FInstancedStruct::Make<decltype(Default)>(Default); }, FName(Category) }

#define DECLARE_UTK_NODE(Name, DisplayName, Category, Pins, Properties, Logic) \
	FUTKNodeDefinition Create_##Name##_NodeDefinition() \
	{ \
		return FUTKNodeDefinition{ \
			#Name, DisplayName, Category, Pins, Properties, Logic \
		}; \
	}

#define REGISTER_UTK_NODE(Name) \
	FUTKNodeFactory::Get().RegisterNode(Create_##Name##_NodeDefinition());