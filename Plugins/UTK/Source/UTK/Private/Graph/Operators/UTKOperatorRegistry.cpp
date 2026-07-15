#include "Graph/Operators/UTKOperatorRegistry.h"

#include "Core/UTKLogger.h"
#include "Graph/Nodes/UTKNodeDefinition.h"

FUTKOperatorRegistry& FUTKOperatorRegistry::Get()
{
	static FUTKOperatorRegistry Instance;
	return Instance;
}

bool FUTKOperatorRegistry::RegisterOperator(const FUTKOperatorDefinition& Definition)
{
	FString Error;

	if (!Definition.IsValid(&Error))
	{
		UE_LOG(
			LogUTKEditor,
			Error,
			TEXT("[UTK] Refused invalid operator registration: %s"),
			*Error);

		return false;
	}

	if (Operators.Contains(Definition.OperatorId))
	{
		UE_LOG(
			LogUTKEditor,
			Error,
			TEXT("[UTK] Duplicate operation registration: %s"),
			*Definition.OperatorId.ToString());

		return false;
	}

	Operators.Add(Definition.OperatorId, Definition);
	return true;
}

const FUTKOperatorDefinition* FUTKOperatorRegistry::FindOperator(FName OperatorId) const
{
	return Operators.Find(OperatorId);
}

bool FUTKOperatorRegistry::ValidateNodeDefinition(const FUTKNodeDefinition& NodeDefinition, FString& OutError) const
{
	const FUTKOperatorDefinition* Operator = FindOperator(NodeDefinition.OperatorId);

	if (!Operator)
	{
		OutError = FString::Printf(TEXT("Unknown operator: %s"), *NodeDefinition.OperatorId.ToString());
		return false;
	}

	TArray<const FUTKNodePinDefinition*> InputPins;
	TArray<const FUTKNodePinDefinition*> OutputPins;

	for (const FUTKNodePinDefinition& Pin : NodeDefinition.Pins)
	{
		(Pin.bInput ? InputPins : OutputPins).Add(&Pin);
	}

	if (InputPins.Num() != Operator->Inputs.Num())
	{
		OutError = FString::Printf(
			TEXT("Node %s' exposes %d inputs, but operator '%s' expects %d."),
			*NodeDefinition.TypeId.ToString(),
			InputPins.Num(),
			*Operator->OperatorId.ToString(),
			Operator->Inputs.Num());

		return false;
	}

	if (OutputPins.Num() != Operator->Outputs.Num())
	{
		OutError = FString::Printf(
			TEXT("Node %s' exposes %d outputs, but operator '%s' expects %d."),
			*NodeDefinition.TypeId.ToString(),
			OutputPins.Num(),
			*Operator->OperatorId.ToString(),
			Operator->Outputs.Num());

		return false;
	}

	for (int32 Index = 0; Index < InputPins.Num(); ++Index)
	{
		const EUTKFieldType NodeType = InputPins[Index]->FieldType;
		const EUTKFieldType OperatorType = Operator->Inputs[Index].FieldType;

		if (NodeType != EUTKFieldType::Any && OperatorType != EUTKFieldType::Any && NodeType != OperatorType)
		{
			OutError = FString::Printf(
				TEXT("Node '%s' input %d does not match operator '%s'."),
				*NodeDefinition.TypeId.ToString(),
				Index,
				*Operator->OperatorId.ToString());

			return false;
		}
	}

	for (int32 Index = 0; Index < OutputPins.Num(); ++Index)
	{
		const EUTKFieldType NodeType = OutputPins[Index]->FieldType;
		const EUTKFieldType OperatorType = Operator->Outputs[Index].FieldType;

		if (NodeType != EUTKFieldType::Any && OperatorType != EUTKFieldType::Any && NodeType != OperatorType)
		{
			OutError = FString::Printf(
				TEXT("Node '%s' output %d does not match operator '%s'."),
				*NodeDefinition.TypeId.ToString(),
				Index,
				*Operator->OperatorId.ToString());

			return false;
		}
	}

	return true;
}

void FUTKOperatorRegistry::Clear()
{
	Operators.Empty();
}