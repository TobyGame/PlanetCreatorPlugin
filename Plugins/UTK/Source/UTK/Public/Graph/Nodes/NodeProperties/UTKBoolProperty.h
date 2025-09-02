#pragma once

#include "CoreMinimal.h"
#include "UTKBoolProperty.generated.h"

USTRUCT(BlueprintType)
struct FUTKBoolProperty
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Property")
	bool Value = false;
};