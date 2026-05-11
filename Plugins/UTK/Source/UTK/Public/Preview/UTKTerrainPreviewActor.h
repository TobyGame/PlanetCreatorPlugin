#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "UTKTerrainPreviewActor.generated.h"

class UUTKTerrainPreviewComponent;

UCLASS()
class UTK_API AUTKTerrainPreviewActor : public AActor
{
	GENERATED_BODY()

public:
	AUTKTerrainPreviewActor();

	UUTKTerrainPreviewComponent* GetPreviewComponent() const { return PreviewComponent; }

private:
	UPROPERTY(VisibleAnywhere, Category="UTK Preview")
	TObjectPtr<UUTKTerrainPreviewComponent> PreviewComponent = nullptr;
};