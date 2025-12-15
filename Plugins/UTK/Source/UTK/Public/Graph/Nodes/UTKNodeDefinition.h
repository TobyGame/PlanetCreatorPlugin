#pragma once

#include "StructUtils/InstancedStruct.h"
#include "Graph/Nodes//UTKNodeExecutionTypes.h"


/**
 * Pin definition used by the generic node system.
 * 
 * The extra fields (DefaultLayerName, bComputeOnlyIfConnected) are added
 * with default values, so existing DEFINE_PIN macro usage keeps compiling.
 */
struct FUTKNodePinDefinition
{
	/** Pin name as shown in the graph */
	FName Name;

	/** True if this pin is an input pin, false if it is an output pin. */
	bool bInput = false;

	/** True if this pin is required for the node to operate correctly */
	bool bRequired = false;

	/**
	 * Default layer name to use when this pin is connected to a FUTKTerrain.
	 * For example, "Height", "Sediment", "Flow", etc.
	 * 
	 * For inputs:
	 *	- If not specified, nodes may assume "Height" or decide per-node.
	 * For outputs:
	 *	- Indicates which layer in the node's terrain the pin exposes.
	 */
	FName DefaultLayerName = NAME_None;

	/**
	 * If true (for output pins), the node is allowed to skip computing this
	 * output when there is no downstream connection. This helps memory/perf.
	 */
	bool bComputeOnlyIfConnected = false;

	FUTKNodePinDefinition() = default;

	/**
	 * Convenience constructor used by DEFINE_PIN. If no explicit DefaultLayerName
	 * is provided for an output pin, the pin is also used as the layer
	 * name, so node implementations don't need to hard-code "Height"/"Mask".
	 */
	FUTKNodePinDefinition(
		FName InName,
		bool bInInput,
		bool bInRequired,
		FName InDefaultLayerName = NAME_None,
		bool bInComputeOnlyIfConnected = false)
		: Name(InName)
		  , bInput(bInInput)
		  , bRequired(bInRequired)
		  , DefaultLayerName(InDefaultLayerName.IsNone() && !bInInput ? InName : InDefaultLayerName)
		  , bComputeOnlyIfConnected(bInComputeOnlyIfConnected)
	{}
};

/**
 * Property definition used by nodes. Properties are backed by FInstancedStruct
 * so that different nodes can declare different struct types as parameters.
 */
struct FUTKNodePropertyDefinition
{
	/** Property name (used as key in the node's RuntimeProperties map). */
	FName Name;

	/** Function that creates a default value instance. */
	TFunction<FInstancedStruct()> Value;

	/** Category name used by the property UI panel. */
	FName Category;
};

/**
 * Complete node definition used by FUTKNodeFactory and UUTKNode.
 * 
 * Name / DisplayName / Category ae used for UI and registration.
 * Pins and Properties drive the graph/editor UI.
 * Process is the core node evaluation lambda using the new execution model.
 */
struct FUTKNodeDefinition
{
	/** Internal type name (e.g., "Constant"). */
	FString Name;

	/** Display name for UI (e.g., "Constant"). */
	FString DisplayName;

	/** Category show in node palettes (e.g. "Math", "Erosion"). */
	FString Category;

	/** All input and output pin definitions for this node. */
	TArray<FUTKNodePinDefinition> Pins;

	/** All property definitions for this node. */
	UClass* SettingsClass = nullptr;

	/** Processing function implementing the node's behavior. */
	FUTKNodeProcessFunction ProcessFunction;
};