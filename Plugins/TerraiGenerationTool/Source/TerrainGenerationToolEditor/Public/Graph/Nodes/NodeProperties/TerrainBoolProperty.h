#pragma once

#include "CoreMinimal.h"
#include "TerrainBoolProperty.generated.h"

USTRUCT(BlueprintType)
struct FTerrainBoolProperty
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Property")
	bool Value = false;
};