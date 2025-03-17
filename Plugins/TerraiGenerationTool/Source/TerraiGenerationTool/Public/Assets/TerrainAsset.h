#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TerrainAsset.generated.h"

/**
 * UTerrainAsset - Custom asset for terrain generation
 */

UCLASS()
class TERRAIGENERATIONTOOL_API UTerrainAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Default constructor*/
	UTerrainAsset();

#if WITH_EDITORONLY_DATA

	/** Example property for terrain setting */
	UPROPERTY(EditAnywhere, Category = "Terrain Settings")
	float TerrainScale = 1.0f;

#endif
};