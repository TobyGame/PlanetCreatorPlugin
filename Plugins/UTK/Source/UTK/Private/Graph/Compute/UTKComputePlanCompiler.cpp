#include "Graph/Compute/UTKComputePlanCompiler.h"

#include "Graph/Nodes/UTKNode.h"
#include "Graph/Nodes/UTKNodeExecutionTypes.h"
#include "Graph/Operators/UTKOperatorRegistry.h"
#include "Graph/UTKGraphEvaluation.h"
#include "Misc/ScopeExit.h"

namespace
{
	struct FUTKCompileOutputKey
	{
		FGuid NodeGuid;
		FName OutputPinName = NAME_None;

		friend bool operator==(const FUTKCompileOutputKey& A, const FUTKCompileOutputKey& B)
		{
			return A.NodeGuid == B.NodeGuid && A.OutputPinName == B.OutputPinName;
		}

		friend uint32 GetTypeHash(const FUTKCompileOutputKey& Key)
		{
			return HashCombine(GetTypeHash(Key.NodeGuid), GetTypeHash(Key.OutputPinName));
		}
	};

	class FUTKInternalComputePlanCompiler
	{
	public:
		FUTKInternalComputePlanCompiler(const FUTKNodeExecutionContext& InContext, FUTKComputePlan& InPlan)
			: Context(InContext), Plan(InPlan)
		{}

		bool CompileRequestedOutput(UUTKNode* Node, FName OutputPinName, FUTKLogicalFieldHandle& OutField, FString& OutError)
		{
			if (!CompileNodeOutput(Node, OutputPinName, OutField, OutError))
				return false;

			if (!Plan.MarkFieldPersistent(OutField))
			{
				OutError = TEXT("Failed to mark the requested output field as persistent.");
				return false;
			}

			FUTKComputePlanRequestOutput RequestOutput;
			RequestOutput.NodeGuid = Node->NodeGuid;
			RequestOutput.OutputPinName = OutputPinName;
			RequestOutput.Field = OutField;

			Plan.AddRequestedOutput(RequestOutput);

			return true;
		}

	private:
		bool CompileNodeOutput(UUTKNode* Node, FName RequestedOutputPinName, FUTKLogicalFieldHandle& OutField, FString& OutError)
		{
			if (!Node)
			{
				OutError = TEXT("Cannot compile a null UTK node.");
				return false;
			}

			const FUTKCompileOutputKey RequestedKey{ Node->NodeGuid, RequestedOutputPinName };

			if (const FUTKLogicalFieldHandle* Existing = CompiledOutputs.Find(RequestedKey))
			{
				OutField = *Existing;
				return true;
			}

			if (ActiveNodes.Contains(Node->NodeGuid))
			{
				OutError = FString::Printf(TEXT("Cycle detected while compiling node '%s'."), *Node->GetNodeTitle(ENodeTitleType::ListView).ToString());
				return false;
			}

			const FUTKNodeDefinition& NodeDefinition = Node->GetDefinition();

			const FUTKNodePinDefinition* RequestedPin = Node->FindPinDefinition(RequestedOutputPinName, EGPD_Output);

			if (!RequestedPin)
			{
				OutError = FString::Printf(TEXT("Node '%s' does not expose output pin '%s'."), *NodeDefinition.TypeId.ToString(), *RequestedOutputPinName.ToString());
				return false;
			}

			const FUTKOperatorDefinition* Operator = FUTKOperatorRegistry::Get().FindOperator(NodeDefinition.OperatorId);

			if (!Operator)
			{
				OutError = FString::Printf(TEXT("Operator '%s' is not registered."), *NodeDefinition.OperatorId.ToString());
				return false;
			}

			ActiveNodes.Add(Node->NodeGuid);

			ON_SCOPE_EXIT{
				ActiveNodes.Remove(Node->NodeGuid);
			};

			TArray<FUTKResolvedInput> ResolvedInputs;

			if (!ResolveInputsForNode(Node, ResolvedInputs))
			{
				OutError = FString::Printf(TEXT("Node '%s' has a missing required input."), *NodeDefinition.TypeId.ToString());
				return false;
			}

			TArray<FUTKLogicalFieldHandle> InputFields;
			InputFields.Reserve(ResolvedInputs.Num());

			for (const FUTKResolvedInput& ResolvedInput : ResolvedInputs)
			{
				FUTKLogicalFieldHandle InputField;

				if (ResolvedInput.UpstreamNode)
				{
					if (!CompileNodeOutput(ResolvedInput.UpstreamNode, ResolvedInput.UpstreamOutputPinName, InputField, OutError))
						return false;
				}
				else if (ResolvedInput.bRequired)
				{
					OutError = FString::Printf(TEXT("Required input '%s' is not connected on node '%s'."), *ResolvedInput.InputPinName.ToString(), *NodeDefinition.TypeId.ToString());
					return false;
				}

				/**
				 * A disconnected optional input is represented by an invalid
				 * logical field handle. Its slot is still preserved.
				 */
				InputFields.Add(InputField);
			}

			FUTKParameterBlock ParameterBlock;

			if (!FUTKOperatorParameterPacker::Pack(Node->GetSettings(), Operator->Parameters, ParameterBlock, OutError))
			{
				OutError = FString::Printf(TEXT("Failed to pack parameters for node '%s': %s"), *NodeDefinition.TypeId.ToString(), *OutError);
				return false;
			}

			TArray<const FUTKNodePinDefinition*> OutputPins;

			for (const FUTKNodePinDefinition& Pin : NodeDefinition.Pins)
			{
				if (!Pin.bInput)
					OutputPins.Add(&Pin);
			}

			if (OutputPins.Num() != Operator->Outputs.Num())
			{
				OutError = FString::Printf(TEXT("Node '%s' output signature no longer matches operator '%s'."), *NodeDefinition.TypeId.ToString(), *Operator->OperatorId.ToString());
				return false;
			}

			TArray<FUTKLogicalFieldHandle> OutputFields;
			OutputFields.Reserve(OutputPins.Num());

			for (int32 OutputIndex = 0; OutputIndex < OutputPins.Num(); ++OutputIndex)
			{
				const FUTKNodePinDefinition& OutputPin = *OutputPins[OutputIndex];

				EUTKFieldType OutputType = OutputPin.FieldType;

				if (OutputType == EUTKFieldType::Any)
					OutputType = Operator->Outputs[OutputIndex].FieldType;

				FUTKFieldDescriptor Descriptor;
				Descriptor.Domain = FUTKDomain2D(Context.ResolutionX, Context.ResolutionY);

				Descriptor.ValueType = OutputType;

				Descriptor.Precision = OutputType == EUTKFieldType::Integer ? EUTKFieldPrecision::UInt32 : EUTKFieldPrecision::Automatic;

				Descriptor.Lifetime = EUTKFieldLifetime::Transient;

				Descriptor.DebugLabel = FString::Printf(TEXT("%s.%s"), *NodeDefinition.TypeId.ToString(), *OutputPin.Name.ToString());

				const FUTKLogicalFieldHandle OutputField = Plan.CreateField(Descriptor);

				if (!OutputField.IsValid())
				{
					OutError = FString::Printf(TEXT("Failed to create logical field for '%s.%s'."), *NodeDefinition.TypeId.ToString(), *OutputPin.Name.ToString());
					return false;
				}

				OutputFields.Add(OutputField);
			}

			FUTKComputePlanOperation Operation;
			Operation.SourceNodeGuid = Node->NodeGuid;
			Operation.SourceNodeType = NodeDefinition.TypeId;
			Operation.OperatorId = Operator->OperatorId;
			Operation.KernelFamily = Operator->KernelFamily;
			Operation.Inputs = MoveTemp(InputFields);
			Operation.Outputs = OutputFields;
			Operation.Parameters = MoveTemp(ParameterBlock);
			Operation.DebugLabel = Node->GetNodeTitle(ENodeTitleType::ListView).ToString();

			if (Plan.AddOperation(MoveTemp(Operation)) == INDEX_NONE)
			{
				OutError = FString::Printf(TEXT("Failed to add compute operation for node '%s'."), *NodeDefinition.TypeId.ToString());
				return false;
			}

			for (int32 OutputIndex = 0; OutputIndex < OutputPins.Num(); ++OutputIndex)
			{
				const FUTKCompileOutputKey OutputKey{ Node->NodeGuid, OutputPins[OutputIndex]->Name };
				CompiledOutputs.Add(OutputKey, OutputFields[OutputIndex]);
			}

			const FUTKLogicalFieldHandle* CompiledRequestedOutput = CompiledOutputs.Find(RequestedKey);

			if (!CompiledRequestedOutput)
			{
				OutError = FString::Printf(TEXT("Requested output '%s' was not compiled for node '%s'."), *RequestedOutputPinName.ToString(), *NodeDefinition.TypeId.ToString());
				return false;
			}

			OutField = *CompiledRequestedOutput;
			return true;
		}

	private:
		const FUTKNodeExecutionContext& Context;

		FUTKComputePlan& Plan;

		TMap<FUTKCompileOutputKey, FUTKLogicalFieldHandle> CompiledOutputs;
		TSet<FGuid> ActiveNodes;
	};
}
bool FUTKComputePlanCompiler::Compile(UUTKNode* Node, FName OutputPinName, const FUTKNodeExecutionContext& Context, FUTKComputePlan& OutPlan, FString& OutError)
{
	OutPlan.Reset();
	OutError.Reset();

	if (!Node)
	{
		OutError = TEXT("Cannot compile a null UTK graph node.");
		return false;
	}

	if (!Context.IsValid())
	{
		OutError = TEXT("Cannot compile a compute plan with an invalid execution domain.");
		return false;
	}

	if (OutputPinName.IsNone())
	{
		OutError = TEXT("A compute-plan output pin name is required.");
		return false;
	}

	FUTKInternalComputePlanCompiler Compiler(Context, OutPlan);

	FUTKLogicalFieldHandle RequestedField;

	if (!Compiler.CompileRequestedOutput(Node, OutputPinName, RequestedField, OutError))
	{
		OutPlan.Reset();
		return false;
	}

	if (!OutPlan.IsValid(&OutError))
	{
		OutPlan.Reset();
		return false;
	}

	return true;
}