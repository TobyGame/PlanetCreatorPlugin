#pragma once

#include "CoreMinimal.h"
#include "Graph/UTKFieldTypes.h"
#include "Graph/Nodes/UTKNodeExecutionTypes.h"

class UUTKNode;

enum class EUTKKernelFamily : uint8
{
	Source,
	Rasterize,
	ProceduralGenerator,
	Pointwise,
	NeighborhoodFilter,
	GlobalAnalysis,
	IterativeSimulation,
	FieldAssembly,
	ProductAssembly,
	Custom,
};

struct FUTKOperatorSlotDefinition
{
	EUTKFieldType FieldType = EUTKFieldType::Scalar;
};

/**
 * Transitional CPU elevator.
 * 
 * This exists only for the current internal prototype fixtures. The GPU complete
 * plan will replace this callback in Phase 4.8A.2.
 */
struct FUTKOperatorReferenceEvaluationRequest
{
	const UUTKNode& Node;

	const TArray<FUTKNodeInput>& Inputs;

	const FUTKNodeExecutionContext& Context;

	FUTKTerrainWorkspace& Workspace;
};

using FUTKOperatorReferenceEvaluator = bool (*)(
	const FUTKOperatorReferenceEvaluationRequest& Request,
	TArray<FUTKNodeOutput>& InOutOutputs,
	FString& OutError);

struct FUTKOperatorDefinition
{
	FName OperatorId = NAME_None;

	EUTKKernelFamily KernelFamily = EUTKKernelFamily::Custom;

	TArray<FUTKOperatorSlotDefinition> Inputs;
	TArray<FUTKOperatorSlotDefinition> Outputs;

	bool bPrototypeOnly = false;
	bool bSupportsPointwiseFusion = false;

	FUTKOperatorReferenceEvaluator ReferenceEvaluator = nullptr;

	bool IsValid(FString* OutError = nullptr) const
	{
		if (OperatorId.IsNone())
		{
			if (OutError)
				*OutError = TEXT("Operator id is required");

			return false;
		}

		if (Outputs.IsEmpty())
		{
			if (OutError)
				*OutError = TEXT("An operator requires at least one output");

			return false;
		}

		return true;
	}
};

class FUTKOperatorSpecBuilder
{
public:
	explicit FUTKOperatorSpecBuilder(FName InOperatorId)
	{
		Definition.OperatorId = InOperatorId;
	}

	FUTKOperatorSpecBuilder& Family(EUTKKernelFamily InFamily)
	{
		Definition.KernelFamily = InFamily;
		return *this;
	}

	FUTKOperatorSpecBuilder& Input(EUTKFieldType FieldType)
	{
		Definition.Inputs.Emplace(FUTKOperatorSlotDefinition{ FieldType });
		return *this;
	}

	FUTKOperatorSpecBuilder& Output(EUTKFieldType FieldType)
	{
		Definition.Outputs.Emplace(FUTKOperatorSlotDefinition{ FieldType });
		return *this;
	}

	FUTKOperatorSpecBuilder& PrototypeOnly()
	{
		Definition.bPrototypeOnly = true;
		return *this;
	}

	FUTKOperatorSpecBuilder& SupportsPointwiseFusion()
	{
		Definition.bSupportsPointwiseFusion = true;
		return *this;
	}

	FUTKOperatorSpecBuilder& ReferenceEvaluator(FUTKOperatorReferenceEvaluator InEvaluator)
	{
		Definition.ReferenceEvaluator = InEvaluator;
		return *this;
	}

	FUTKOperatorDefinition Build() const
	{
		return Definition;
	}

private:
	FUTKOperatorDefinition Definition;
};