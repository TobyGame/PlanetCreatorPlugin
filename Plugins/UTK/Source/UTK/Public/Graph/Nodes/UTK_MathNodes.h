#pragma once

#include "CoreMinimal.h"
#include "UTKNodeSettings.h"
#include "UTK_MathNodes.generated.h"

UCLASS()
class UUTKConstantSettings : public UUTKNodeSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Constant", meta=(ClampMin="0.0", ClampMax="1.0"))
	float Value = 1.0f;
};

UCLASS()
class UUTKCombineSettings : public UUTKNodeSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Combine", meta=(ClampMin="0.0", ClampMax="1.0"))
	float Ratio = 0.5f;

	UPROPERTY(EditAnywhere, Category="Combine")
	bool bSwapInputs = false;
};

void RegisterMathNodes();