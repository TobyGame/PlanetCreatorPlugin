#pragma once

#include "CoreMinimal.h"
#include "UTKNodeSettings.h"
#include "Graph/UTKFieldTypes.h"


/**
 * Logical pin definition.
 * 
 * DefaultLayerName is field routing metadata. It is not a hard-coded height,
 * mask, or material convention.
 */
struct FUTKNodePinDefinition
{
	FName Name = NAME_None;

	EUTKFieldType FieldType = EUTKFieldType::Scalar;

	bool bInput = false;
	bool bRequired = false;

	FName DefaultLayerName = NAME_None;

	bool bComputeOnlyIfConnected = false;

	FUTKNodePinDefinition() = default;

	FUTKNodePinDefinition(
		FName InName,
		EUTKFieldType InFieldType,
		bool bInInput,
		bool bInRequired,
		FName InDefaultLayerName = NAME_None,
		bool bInComputeOnlyIfConnected = false)
		: Name(InName)
		  , FieldType(InFieldType)
		  , bInput(bInInput)
		  , bRequired(bInRequired)
		  , DefaultLayerName(InDefaultLayerName.IsNone() && !bInInput ? InName : InDefaultLayerName)
		  , bComputeOnlyIfConnected(bInComputeOnlyIfConnected)
	{}
};

/**
 * Declarative node definition.
 * 
 * Production nodes describe their intent only. They do not allocate buffers,
 * create texture, enqueue RDG work, or contain evaluation loops
 */
struct FUTKNodeDefinition
{
	FName TypeId = NAME_None;

	FText DisplayName;
	FText Category;
	FText Tooltip;

	TArray<FUTKNodePinDefinition> Pins;

	TSubclassOf<UUTKNodeSettings> SettingsClass;

	/** Resolved by FUTKOperatorRegistry during evaluation */
	FName OperatorId = NAME_None;

	EUTKNodeExposure Exposure = EUTKNodeExposure::Production;

	bool IsValid(FString* OutError = nullptr) const
	{
		auto Fail = [OutError](const TCHAR* Message){
			if (OutError)
				*OutError = Message;

			return false;
		};

		if (TypeId.IsNone())
			return Fail(TEXT("Node type id is required."));

		if (OperatorId.IsNone())
			return Fail(TEXT("Node operator id is required."));

		TSet<FName> SeenPins;

		for (const FUTKNodePinDefinition& Pin : Pins)
		{
			if (Pin.Name.IsNone())
				return Fail(TEXT("Node pin name is required."));

			if (SeenPins.Contains(Pin.Name))
				return Fail(TEXT("Node pin names must be unique."));

			SeenPins.Add(Pin.Name);
		}

		return true;
	}
};

class FUTKNodeSpecBuilder
{
public:
	explicit FUTKNodeSpecBuilder(FName InTypeId)
	{
		Definition.TypeId = InTypeId;
	}

	FUTKNodeSpecBuilder& DisplayName(const TCHAR* InDisplayName)
	{
		Definition.DisplayName = FText::FromString(InDisplayName);
		return *this;
	}

	FUTKNodeSpecBuilder& Category(const TCHAR* InCategory)
	{
		Definition.Category = FText::FromString(InCategory);
		return *this;
	}

	FUTKNodeSpecBuilder& Tooltip(const TCHAR* InTooltip)
	{
		Definition.Tooltip = FText::FromString(InTooltip);
		return *this;
	}

	template <typename TSettings>
	FUTKNodeSpecBuilder& Settings()
	{
		Definition.SettingsClass = TSettings::StaticClass();
		return *this;
	}

	FUTKNodeSpecBuilder& Input(FName Name, EUTKFieldType FieldType, bool bRequired = true, FName DefaultLayerName = NAME_None)
	{
		Definition.Pins.Emplace(Name, FieldType, true, bRequired, DefaultLayerName);
		return *this;
	}

	FUTKNodeSpecBuilder& Output(FName Name, EUTKFieldType FieldType, bool bComputeOnlyIfConnected = false, FName DefaultLayerName = NAME_None)
	{
		Definition.Pins.Emplace(Name, FieldType, false, false, DefaultLayerName, bComputeOnlyIfConnected);
		return *this;
	}

	FUTKNodeSpecBuilder& Operator(FName InOperatorId)
	{
		Definition.OperatorId = InOperatorId;
		return *this;
	}

	FUTKNodeSpecBuilder& Internal()
	{
		Definition.Exposure = EUTKNodeExposure::Internal;
		return *this;
	}

	FUTKNodeDefinition Build() const
	{
		return Definition;
	}

private:
	FUTKNodeDefinition Definition;
};