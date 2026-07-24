#pragma once

#include "CoreMinimal.h"

class UUTKNodeSettings;

/**
 * Parameter types supported by the generic node parameters packer.
 * 
 * Additional data types such as curve, gradients, textures, and spline data
 * will later use dedicated resource binding rather than being forced into
 * this numeric block.
 */
enum class EUTKParameterType : uint8
{
	Float,
	Int32,
	Bool,
	Vector2,
	Vector3,
	Vector4,
	Color,
};

/**
 * Defines how one parameter is read from its node settings object.
 */
struct UTK_API FUtkOperatorParametersDefinition
{
	/** Name used by the logical compute operation and future GPU kernel. */
	FName ParameterName = NAME_None;

	/** Reflected property name in the node's settings UObject. */
	FName SettingsPropertyName = NAME_None;

	EUTKParameterType Type = EUTKParameterType::Float;

	bool IsValid() const
	{
		return !ParameterName.IsNone() && !SettingsPropertyName.IsNone();
	}
};

/**
 * One packed numeric parameter.
 * 
 * Float and vector values use FloatValue. Integer, bool, end enum values use
 * IntegerValue. Keeping these representations separate avoids converting
 * integers to floating point before the GPU parameter layout is defined.
 */
struct UTK_API FUTKPackedParameter
{
	FName Name = NAME_None;

	EUTKParameterType Type = EUTKParameterType::Float;

	FVector4f FloatValue = FVector4f::Zero();

	int32 IntegerValue = 0;
};

/**
 * Ordered parameter block attached to one compute-plan operation.
 */
struct UTK_API FUTKParameterBlock
{
	TArray<FUTKPackedParameter> Values;

	void Reset()
	{
		Values.Reset();
	}

	bool IsEmpty() const
	{
		return Values.IsEmpty();
	}

	const FUTKPackedParameter* Find(FName Name) const
	{
		return Values.FindByPredicate(
			[Name](const FUTKPackedParameter& Parameter){
				return Parameter.Name == Name;
			});
	}

	FUTKPackedParameter* Find(FName Name)
	{
		return Values.FindByPredicate(
			[Name](FUTKPackedParameter& Parameter){
				return Parameter.Name == Name;
			});
	}
};

/**
 * Centralized reflected-property reader.
 * 
 * Nodes do not write custom parameter extraction code. Their operator
 * descriptors only bind logical parameter names to settings property names.
 */
class UTK_API FUTKOperatorParameterPacker
{
public:
	static bool ValidateDefinitions(TSubclassOf<UUTKNodeSettings> SettingsClass, const TArray<FUtkOperatorParametersDefinition>& Definitions, FString& OutError);

	static bool Pack(const UUTKNodeSettings* Settings, const TArray<FUtkOperatorParametersDefinition>& Definitions, FUTKParameterBlock& OutBlock, FString& OutError);
};