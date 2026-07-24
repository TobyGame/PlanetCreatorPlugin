#include "Graph/Compute/UTKComputePlan.h"

void FUTKComputePlan::Reset()
{
	Fields.Reset();
	Operations.Reset();
	RequestedOutputs.Reset();
}

FUTKLogicalFieldHandle FUTKComputePlan::CreateField(const FUTKFieldDescriptor& Descriptor)
{
	if (!Descriptor.IsValid())
		return FUTKLogicalFieldHandle();

	const int32 NewIndex = Fields.Add(Descriptor);

	return FUTKLogicalFieldHandle(NewIndex);
}

int32 FUTKComputePlan::AddOperation(FUTKComputePlanOperation&& Operation)
{
	if (!Operation.IsValid())
		return INDEX_NONE;

	return Operations.Add(MoveTemp(Operation));
}

bool FUTKComputePlan::MarkFieldPersistent(FUTKLogicalFieldHandle Field)
{
	FUTKFieldDescriptor* Descriptor = GetField(Field);

	if (!Descriptor)
		return false;

	Descriptor->Lifetime = EUTKFieldLifetime::Persistent;
	return true;
}

void FUTKComputePlan::AddRequestedOutput(const FUTKComputePlanRequestOutput& Output)
{
	if (Output.IsValid())
		RequestedOutputs.Add(Output);
}

const FUTKFieldDescriptor* FUTKComputePlan::GetField(FUTKLogicalFieldHandle Field) const
{
	return Fields.IsValidIndex(Field.Index) ? &Fields[Field.Index] : nullptr;
}

FUTKFieldDescriptor* FUTKComputePlan::GetField(FUTKLogicalFieldHandle Field)
{
	return Fields.IsValidIndex(Field.Index) ? &Fields[Field.Index] : nullptr;
}

int32 FUTKComputePlan::GetPersistentFieldCount() const
{
	int32 Count = 0;

	for (const FUTKFieldDescriptor& Field : Fields)
	{
		if (Field.Lifetime == EUTKFieldLifetime::Persistent)
			++Count;
	}

	return Count;
}

bool FUTKComputePlan::IsValid(FString* OutError) const
{
	auto Fail = [OutError](const FString& Message){
		if (OutError)
			*OutError = Message;

		return false;
	};

	if (Operations.IsEmpty())
		return Fail(TEXT("The compute plan contains no operations."));

	if (RequestedOutputs.IsEmpty())
		return Fail(TEXT("The compute plan contains no requested outputs."));

	for (int32 FieldIndex = 0; FieldIndex < Fields.Num(); ++FieldIndex)
	{
		if (!Fields[FieldIndex].IsValid())
			return Fail(FString::Printf(TEXT("Logical field %d is invalid."), FieldIndex));
	}

	for (int32 OperationIndex = 0; OperationIndex < Operations.Num(); ++OperationIndex)
	{
		const FUTKComputePlanOperation& Operation = Operations[OperationIndex];

		if (!Operation.IsValid())
			return Fail(FString::Printf(TEXT("Compute operation %d is invalid."), OperationIndex));

		for (const FUTKLogicalFieldHandle Input : Operation.Inputs)
		{
			/**
			 * Invalid input handles are permitted for disconected optional input slots.
			 */
			if (Input.IsValid() && !Fields.IsValidIndex(Input.Index))
				return Fail(FString::Printf(TEXT("Compute operation %d references an invalid input field."), OperationIndex));
		}

		for (const FUTKLogicalFieldHandle Output : Operation.Outputs)
		{
			if (!Fields.IsValidIndex(Output.Index))
				return Fail(FString::Printf(TEXT("Compute operation %d references an invalid output field."), OperationIndex));
		}
	}

	for (const FUTKComputePlanRequestOutput& RequestOutput : RequestedOutputs)
	{
		if (!RequestOutput.IsValid() || !Fields.IsValidIndex(RequestOutput.Field.Index))
			return Fail(TEXT("A requested compute-plan output is invalid."));

		if (Fields[RequestOutput.Field.Index].Lifetime != EUTKFieldLifetime::Persistent)
			return Fail(TEXT("A requested output was not marked as a persisted field."));
	}

	return true;
}