#pragma once

#include "CoreMinimal.h"
#include "UTKFloatProperty.generated.h"

USTRUCT(BlueprintType)
struct FUTKFloatProperty
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Property")
	float Value = 0.f;
};