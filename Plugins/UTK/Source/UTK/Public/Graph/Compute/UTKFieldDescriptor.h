#pragma once

#include "CoreMinimal.h"
#include "Core/UTKTerrainTypes.h"
#include "Graph/UTKFieldTypes.h"

/**
 * Requested numeric precision for a logical field.
 * 
 * Automatic allows the future GPU allocator to select an appropriate physical
 * format from the logical field 
 */
enum class EUTKFieldPrecision : uint8
{
	Automatic,
	Float16,
	Float32,
	UInt32,
};

/**
 * Lifetime requested by the compiled plan.
 * 
 * Transient fields only need to exist during one compute-plan execution.
 * Persistent fields survive execution because they are required by the
 * viewport, a cache, or a build/export operation.
 */
enum class EUTKFieldLifetime : uint8
{
	Transient,
	Persistent,
};

/**
 * Lightweight identifier used inside a compiled compute plan.
 * 
 * It does not own a CPU buffer, texture, render target, or RDG resource.
 */
struct UTK_API FUTKLogicalFieldHandle
{
	int32 Index = INDEX_NONE;

	FUTKLogicalFieldHandle() = default;

	explicit FUTKLogicalFieldHandle(int32 InIndex)
		: Index(InIndex)
	{}

	bool IsValid() const
	{
		return Index != INDEX_NONE;
	}

	void Reset()
	{
		Index = INDEX_NONE;
	}

	friend bool operator==(const FUTKLogicalFieldHandle& A, const FUTKLogicalFieldHandle& B)
	{
		return A.Index == B.Index;
	}

	friend bool operator!=(const FUTKLogicalFieldHandle& A, const FUTKLogicalFieldHandle& B)
	{
		return A.Index != B.Index;
	}

	friend uint32 GetTypeHash(const FUTKLogicalFieldHandle& Handle)
	{
		return GetTypeHash(Handle.Index);
	}
};

/**
 * Logical descriptor of one field inside a compute plan.
 * 
 * This describes what the field contains, not how it is physically stored.
 */
struct UTK_API FUTKFieldDescriptor
{
	FUTKDomain2D Domain;

	EUTKFieldType ValueType = EUTKFieldType::Scalar;
	EUTKFieldPrecision Precision = EUTKFieldPrecision::Automatic;
	EUTKFieldLifetime Lifetime = EUTKFieldLifetime::Transient;

	/** Debug-only label. It is not used for routing or serialization. */
	FString DebugLabel;

	bool IsValid() const
	{
		return Domain.IsValid() && ValueType != EUTKFieldType::Any;
	}

	int32 GetChannelCount() const
	{
		switch (ValueType)
		{
		case EUTKFieldType::Scalar:
		case EUTKFieldType::Integer:
			return 1;

		case EUTKFieldType::Vector2:
			return 2;

		case EUTKFieldType::Vector3:
			return 3;

		case EUTKFieldType::Color:
			return 4;

		default:
			return 0;
		}
	}
};