#pragma once

#include "CoreMinimal.h"
#include "TerrainFloatProperty.generated.h"

USTRUCT(BlueprintType)
struct FTerrainFloatProperty
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Property")
	float Value = 0.f;
};