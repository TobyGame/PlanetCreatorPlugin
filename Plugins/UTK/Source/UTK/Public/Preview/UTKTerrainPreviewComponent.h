#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Preview/UTKPreviewTerrainTypes.h"

#include "UTKTerrainPreviewComponent.generated.h"

struct FUTKTerrain;

UCLASS(ClassGroup=(UTK), meta=(BlueprintSpawnableComponent))
class UTK_API UUTKTerrainPreviewComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	UUTKTerrainPreviewComponent();

	void ClearPreview();

	void UpdateFromTerrain(
		const FUTKTerrain& Terrain,
		FName LayerName,
		const FUTKPreviewTerrainMapping& Mapping);

	bool HasValidPreview() const { return bHasValidPreview; }

	FName GetCurrentLayerName() const { return CurrentLayerName; }
	const FUTKPreviewTerrainMapping& GetCurrentMapping() const { return CurrentMapping; }

	EUTKPreviewBackend GetActiveBackendType() const { return ActiveBackendType; }
	FBoxSphereBounds GetPreviewBounds() const;

private:
	bool bHasValidPreview = false;

	FName CurrentLayerName = NAME_None;
	FUTKPreviewTerrainMapping CurrentMapping;

	EUTKPreviewBackend ActiveBackendType = EUTKPreviewBackend::None;
};