#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UTKAsset.generated.h"

class UUTKGraph;

/**
 * UUTKAsset - Custom asset for terrain generation
 */

UCLASS()
class UTK_API UUTKAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Default constructor*/
	UUTKAsset();

	/** Example property for terrain setting */
	UPROPERTY(EditAnywhere, Category = "Terrain Settings")
	float TerrainScale = 1.0f;


	UPROPERTY(EditAnywhere, Category="UTK|Preview")
	int32 PreviewResolution = 512;

	UPROPERTY(EditAnywhere, Category="UTK|Preview")
	int32 PreviewSeed = 0;

	UPROPERTY(Instanced)
	TObjectPtr<UUTKGraph> Graph = nullptr;
};