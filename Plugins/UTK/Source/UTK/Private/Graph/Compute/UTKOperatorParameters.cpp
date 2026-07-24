#include "Graph/Compute/UTKOperatorParameters.h"

#include "Graph/Nodes/UTKNodeSettings.h"
#include "UObject/UnrealType.h"

namespace
{
	bool IsStructType(const FProperty* Property, const UScriptStruct* ExpectedStruct)
	{
		const FStructProperty* StructProperty = CastField<FStructProperty>(Property);
		return StructProperty && StructProperty->Struct == ExpectedStruct;
	}

	bool IsCompatibleProperty(const FProperty* Property, EUTKParameterType ExpectedType)
	{
		if (!Property)
			return false;

		switch (ExpectedType)
		{

		case EUTKParameterType::Float:
		{
			const FNumericProperty* NumericProperty = CastField<FNumericProperty>(Property);
			return NumericProperty && NumericProperty->IsFloatingPoint();
		}

		case EUTKParameterType::Int32:
		{
			if (CastField<FEnumProperty>(Property))
				return true;

			const FNumericProperty* NumericProperty = CastField<FNumericProperty>(Property);
			return NumericProperty && NumericProperty->IsInteger();
		}

		case EUTKParameterType::Bool:
			return CastField<FBoolProperty>(Property) != nullptr;

		case EUTKParameterType::Vector2:
			return IsStructType(Property, TBaseStructure<FVector2D>::Get());

		case EUTKParameterType::Vector3:
			return IsStructType(Property, TBaseStructure<FVector>::Get());

		case EUTKParameterType::Vector4:
			return IsStructType(Property, TBaseStructure<FVector4>::Get());

		case EUTKParameterType::Color:
			return IsStructType(Property, TBaseStructure<FLinearColor>::Get());

		default:
			return false;
		}
	}

	bool ReadPropertyValue(const UUTKNodeSettings* Settings, const FProperty* Property, const FUtkOperatorParametersDefinition& Definition, FUTKPackedParameter& OutParameter, FString& OutError)
	{
		if (!Settings || !Property)
		{
			OutError = TEXT("Cannot read a parameter from a null settings object.");
			return false;
		}

		const void* ValueAddress = Property->ContainerPtrToValuePtr<void>(Settings);

		if (!ValueAddress)
		{
			OutError = FString::Printf(TEXT("Could not access settings property '%s'."), *Definition.SettingsPropertyName.ToString());
			return false;
		}

		OutParameter.Name = Definition.ParameterName;
		OutParameter.Type = Definition.Type;

		switch (Definition.Type)
		{
		case EUTKParameterType::Float:
		{
			const FNumericProperty* NumericProperty = CastFieldChecked<FNumericProperty>(Property);
			OutParameter.FloatValue.X = static_cast<float>(NumericProperty->GetFloatingPointPropertyValue(ValueAddress));
			return true;
		}

		case EUTKParameterType::Int32:
		{
			if (const FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
			{
				const FNumericProperty* UnderlyingProperty = EnumProperty->GetUnderlyingProperty();
				OutParameter.IntegerValue = static_cast<int32>(UnderlyingProperty->GetSignedIntPropertyValue(ValueAddress));
				return true;
			}

			const FNumericProperty* NumericProperty = CastFieldChecked<FNumericProperty>(Property);
			OutParameter.IntegerValue = static_cast<int32>(NumericProperty->GetSignedIntPropertyValue(ValueAddress));
			return true;
		}

		case EUTKParameterType::Bool:
		{
			const FBoolProperty* BoolProperty = CastFieldChecked<FBoolProperty>(Property);
			OutParameter.IntegerValue = BoolProperty->GetPropertyValue(ValueAddress) ? 1 : 0;
			return true;
		}

		case EUTKParameterType::Vector2:
		{
			const FVector2D& Value = *static_cast<const FVector2D*>(ValueAddress);
			OutParameter.FloatValue = FVector4f(Value.X, Value.Y, 0.0f, 0.0f);
			return true;
		}

		case EUTKParameterType::Vector3:
		{
			const FVector& Value = *static_cast<const FVector*>(ValueAddress);
			OutParameter.FloatValue = FVector4f(Value.X, Value.Y, Value.Z, 0.0f);
			return true;
		}

		case EUTKParameterType::Vector4:
		{
			const FVector4& Value = *static_cast<const FVector4*>(ValueAddress);
			OutParameter.FloatValue = FVector4f(Value.X, Value.Y, Value.Z, Value.W);
			return true;
		}

		case EUTKParameterType::Color:
		{
			const FLinearColor& Value = *static_cast<const FLinearColor*>(ValueAddress);
			OutParameter.FloatValue = FVector4f(Value.R, Value.G, Value.B, Value.A);
			return true;
		}

		default:
			OutError = TEXT("Unsupported UTK operator parameter type.");
			return false;
		}
	}
}

bool FUTKOperatorParameterPacker::ValidateDefinitions(TSubclassOf<UUTKNodeSettings> SettingsClass, const TArray<FUtkOperatorParametersDefinition>& Definitions, FString& OutError)
{
	if (Definitions.IsEmpty())
		return true;

	UClass* ResolvedSettingsClass = SettingsClass.Get();

	if (!ResolvedSettingsClass)
	{
		OutError = TEXT("The operator declares parameters, but the node has no settings class.");
		return false;
	}

	TSet<FName> SeenParameterNames;

	for (const FUtkOperatorParametersDefinition& Definition : Definitions)
	{
		if (!Definition.IsValid())
		{
			OutError = TEXT("An operator parameter definition si invalid.");
			return false;
		}

		if (SeenParameterNames.Contains(Definition.ParameterName))
		{
			OutError = FString::Printf(TEXT("Duplicate logical operator parameter '%s'."), *Definition.ParameterName.ToString());
			return false;
		}

		SeenParameterNames.Add(Definition.ParameterName);

		const FProperty* Property = FindFProperty<FProperty>(ResolvedSettingsClass, Definition.SettingsPropertyName);

		if (!Property)
		{
			OutError = FString::Printf(TEXT("Settings property '%s' was not found on '%s'."), *Definition.SettingsPropertyName.ToString(), *ResolvedSettingsClass->GetName());
			return false;
		}

		if (!IsCompatibleProperty(Property, Definition.Type))
		{
			OutError = FString::Printf(TEXT("Settings property '%s' is incompatible with UTK parameter '%s'."), *Definition.SettingsPropertyName.ToString(), *Definition.ParameterName.ToString());
			return false;
		}
	}

	return true;
}

bool FUTKOperatorParameterPacker::Pack(const UUTKNodeSettings* Settings, const TArray<FUtkOperatorParametersDefinition>& Definitions, FUTKParameterBlock& OutBlock, FString& OutError)
{
	OutBlock.Reset();

	if (Definitions.IsEmpty())
		return true;

	if (!Settings)
	{
		OutError = TEXT("The operator requires parameters, but the node settings object is null.");
		return false;
	}

	if (!ValidateDefinitions(Settings->GetClass(), Definitions, OutError))
		return false;

	OutBlock.Values.Reserve(Definitions.Num());

	for (const FUtkOperatorParametersDefinition& Definition : Definitions)
	{
		const FProperty* Property = FindFProperty<FProperty>(Settings->GetClass(), Definition.SettingsPropertyName);

		FUTKPackedParameter PackedParameter;

		if (!ReadPropertyValue(Settings, Property, Definition, PackedParameter, OutError))
		{
			OutBlock.Reset();
			return false;
		}

		OutBlock.Values.Add(MoveTemp(PackedParameter));
	}

	return true;
}