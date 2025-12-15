#pragma once

#include "CoreMinimal.h"
#include "Graph/Nodes/UTKNodeDefinition.h"
#include "Nodes/UTKNode.h"

class UUTKNode;

/**
 * Resolved description of a single input pin on a node.
 * 
 * This is the adapter between the editor graph (UEdGraph pins) and
 * the evaluation system. It does not expose UEdGraphPin directly, so
 * it can be used by runtime evaluation code without editor types.
 */
struct UTK_API FUTKResolvedInput
{
	/** Logical input pin name on this node (from FUTKNodePinDefinition::Name). */
	FName InputPinName;

	/** Upstream node providing data for this input. Null if disconnected */
	UUTKNode* UpstreamNode = nullptr;

	/** Output pin name on the upstream node we read from. */
	FName UpstreamOutputPinName = NAME_None;

	/** Logical default layer name for this connection (e.g., "Height", "Mask"). */
	FName DefaultLayerName = NAME_None;

	/** Whether this input is required by the node definition. */
	bool bRequired = false;
};

/**
 * Resolve all logical input pins for the given node into upstream
 * connections + metadata (layer, required, etc.).
 * 
 * @param Node: UTK graph node whose inputs we want to resolve.
 * @param OutInputs: one entry per logical input pin defined in FUTKNodeDefinition
 * 
 * @return :
 *  - true if all required inputs are connected
 *  - false if at least one required input has no upstream connection
 *  
 *  @note - This function is the only place that inspects UEdGrapPin.
 *  @note - Evaluation code can later use FUTKResolveInput to build FUTKNodeInput
 */
bool UTK_API ResolveInputsForNode(
	const UUTKNode* Node,
	TArray<FUTKResolvedInput>& OutInputs);


/**
 * Evaluate the given output pin on a node, recursively pulling all dependencies.
 * 
 * @param Node : the graph node to evaluate
 * @param OutputPinName : which output pin to request (e.g., "Result")
 * @param Ctx : execution context (resolution, seed, revisions)
 * 
 * @return Returns a shared pointer to FUTKTerrain or null on error
 */
TSharedPtr<FUTKTerrain> UTK_API EvaluateNodeOutput(
	UUTKNode* Node,
	FName OutputPinName,
	FUTKNodeExecutionContext& Ctx);