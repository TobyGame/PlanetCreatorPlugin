#pragma once

#include "CoreMinimal.h"
#include "Graph/Compute/UTKComputePlan.h"

class UUTKNode;
struct FUTKNodeExecutionContext;

/**
 * Compiles a UTK graph output into a logical, resource-independent plan.
 */
class UTK_API FUTKComputePlanCompiler
{
public:
	static bool Compile(UUTKNode* Node, FName OutputPinName, const FUTKNodeExecutionContext& Context, FUTKComputePlan& OutPlan, FString& OutError);
};