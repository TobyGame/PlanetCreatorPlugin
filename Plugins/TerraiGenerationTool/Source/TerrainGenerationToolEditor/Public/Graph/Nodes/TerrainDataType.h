#pragma once

#include "CoreMinimal.h"
#include "TerrainDataType.generated.h"

UENUM(BlueprintType)
enum class ETerrainDataType : uint8
{
	Heightmap,
	Mask,
	Flow,
	Debris,
	Water,
	Unknown
};