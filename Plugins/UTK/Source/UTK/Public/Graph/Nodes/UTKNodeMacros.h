#pragma once

#include "UTKNodeDefinition.h"
#include "UTKNodeFactory.h"

#define UTK_DEFINE_NODE(NodeName, SpecExpression) \
	FUTKNodeDefinition Create_##NodeName##_NodeDefinition() \
	{ \
		return (SpecExpression).Build(); \
	}

/**
 * Convenience macro to register a node with the global FUTKNodeFactory.
 *
 * Usage:
 *   REGISTER_UTK_NODE(Constant)
 */
#define REGISTER_UTK_NODE(Name) \
	FUTKNodeFactory::Get().RegisterNode(Create_##Name##_NodeDefinition());