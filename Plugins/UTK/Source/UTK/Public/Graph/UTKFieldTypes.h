#pragma once

#include "CoreMinimal.h"

/**
 * Logical data type transported by a UTK graph pin.
 * 
 * This is intentionally independent from physical CPU buffers and eventual GPU
 * texture formats. Height, mask, density, sediment, and moisture are all
 * scalar fields with different semantic usage.
 */
enum class EUTKFieldType : uint8
{
	Any,
	Scalar,
	Vector2,
	Vector3,
	Color,
	Integer,
};

/**
 * Controls whether the node belongs in the normal production palette.
 * 
 * Internal fixtures remain available for migration and validation while the
 * final production node library is built.
 */
enum class EUTKNodeExposure : uint8
{
	Production,
	Internal,
};