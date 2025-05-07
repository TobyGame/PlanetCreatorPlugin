#pragma once

#include "CoreMinimal.h"
#include "TerrainIntProperty.generated.h"

USTRUCT(BlueprintType)
struct FTerrainIntProperty
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Property")
	int32 Value = 0;
};