#pragma once

#include "CoreMinimal.h"
#include "UTKIntProperty.generated.h"

USTRUCT(BlueprintType)
struct FUTKIntProperty
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Property")
	int32 Value = 0;
};