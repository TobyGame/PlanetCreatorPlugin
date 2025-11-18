#pragma once

#include "UTKNodeDefinition.h"
#include "UTKNodeFactory.h"

/**
 * Helper macro to define a pin entry in the Pins array of FUTKNodeDefinition.
 *
 * Usage:
 *   DEFINE_PIN("Result", false, true)
 *
 * The extra fields (DefaultLayerName, bComputeOnlyIfConnected) use defaults
 * from FUTKNodePinDefinition and can be elaborated later with new macros.
 */
#define DEFINE_PIN(Name, bInput, bRequired) \
	FUTKNodePinDefinition{ FName(Name), bInput, bRequired },

/**
 * Helper macro to define a property entry in the Properties array.
 *
 * Usage:
 *   DEFINE_PROPERTY("Value", FUTKFloatProperty(1.0f), "Default")
 *
 * The Default parameter is any struct type that can be stored in FInstancedStruct.
 */
#define DEFINE_PROPERTY(Name, Default, Category) \
	FUTKNodePropertyDefinition{ \
		FName(Name), \
		[]() { return FInstancedStruct::Make<decltype(Default)>(Default); }, \
		FName(Category) \
	},

/**
 * Main macro to declare a UTK node definition.
 *
 * Usage pattern:
 *
 *   DECLARE_UTK_NODE(
 *       Constant,
 *       "Constant",
 *       "Math",
 *       {
 *           DEFINE_PIN("Result", false, true),
 *       },
 *       {
 *           DEFINE_PROPERTY("Value", FUTKFloatProperty(0.0f), "Default"),
 *       },
 *       /* Logic lambda matching FUTKNodeProcessFunction * /
 *       ([](const TArray<FUTKNodeInput>& Inputs,
 *           TArray<FUTKNodeOutput>& Outputs,
 *           FUTKNodeExecutionContext& Ctx,
 *           FUTKTerrainWorkspace& Workspace)
 *       {
 *           // Node implementation here
 *       })
 *   );
 *
 * This macro creates a function Create_<Name>_NodeDefinition() that the
 * factory can use to register the node type.
 */
#define DECLARE_UTK_NODE(NodeName, NodeDisplayName, NodeCategory, PinList, PropertyList, LogicBody) \
	FUTKNodeDefinition Create_##NodeName##_NodeDefinition() \
	{ \
		FUTKNodeDefinition Definition; \
		Definition.Name        = #NodeName; \
		Definition.DisplayName = NodeDisplayName; \
		Definition.Category    = NodeCategory; \
		Definition.Pins        = TArray<FUTKNodePinDefinition>{ PinList }; \
		Definition.Properties  = TArray<FUTKNodePropertyDefinition>{ PropertyList }; \
		Definition.ProcessFunction = LogicBody; \
		return Definition; \
	}


/**
 * Convenience macro to register a node with the global FUTKNodeFactory.
 *
 * Usage:
 *   REGISTER_UTK_NODE(Constant)
 */
#define REGISTER_UTK_NODE(Name) \
	FUTKNodeFactory::Get().RegisterNode(Create_##Name##_NodeDefinition());