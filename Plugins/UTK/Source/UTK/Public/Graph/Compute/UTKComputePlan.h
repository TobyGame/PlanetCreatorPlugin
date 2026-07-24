#pragma once

#include "CoreMinimal.h"
#include "Graph/Compute/UTKFieldDescriptor.h"
#include "Graph/Operators/UTKOperatorDefinition.h"

/**
 * One logical operation inside a compiled UTK compute plan.
 * 
 * The operation references logical field handles only. The future GPU executor
 * resolves those handles into RDG or persistent field surfaces.
 */
struct UTK_API FUTKComputePlanOperation
{
	FGuid SourceNodeGuid;

	FName SourceNodeType = NAME_None;

	FName OperatorId = NAME_None;

	EUTKKernelFamily KernelFamily = EUTKKernelFamily::Custom;

	TArray<FUTKLogicalFieldHandle> Inputs;
	TArray<FUTKLogicalFieldHandle> Outputs;

	FUTKParameterBlock Parameters;

	FString DebugLabel;

	bool IsValid() const
	{
		if (!SourceNodeGuid.IsValid() || OperatorId.IsNone() || Outputs.IsEmpty())
			return false;

		for (const FUTKLogicalFieldHandle Output : Outputs)
		{
			if (!Output.IsValid())
				return false;
		}

		return true;
	}
};

/**
 * Identifies a graph output requested by compute-plan caller.
 */
struct UTK_API FUTKComputePlanRequestOutput
{
	FGuid NodeGuid;

	FName OutputPinName = NAME_None;

	FUTKLogicalFieldHandle Field;

	bool IsValid() const
	{
		return NodeGuid.IsValid() && !OutputPinName.IsNone() && Field.IsValid();
	}
};

/**
 * Resource-independent compiled graph representation.
 */
class UTK_API FUTKComputePlan
{
public:
	void Reset();

	FUTKLogicalFieldHandle CreateField(const FUTKFieldDescriptor& Descriptor);
	int32 AddOperation(FUTKComputePlanOperation&& Operation);
	bool MarkFieldPersistent(FUTKLogicalFieldHandle Field);
	void AddRequestedOutput(const FUTKComputePlanRequestOutput& Output);

	const FUTKFieldDescriptor* GetField(FUTKLogicalFieldHandle Field) const;
	FUTKFieldDescriptor* GetField(FUTKLogicalFieldHandle Field);

	const TArray<FUTKFieldDescriptor>& GetFields() const { return Fields; }
	const TArray<FUTKComputePlanOperation>& GetOperations() const { return Operations; }
	const TArray<FUTKComputePlanRequestOutput>& GetRequestedOutputs() const { return RequestedOutputs; }

	int32 GetPersistentFieldCount() const;
	bool IsValid(FString* OutError = nullptr) const;

private:
	TArray<FUTKFieldDescriptor> Fields;
	TArray<FUTKComputePlanOperation> Operations;
	TArray<FUTKComputePlanRequestOutput> RequestedOutputs;
};