#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Preview/UTKPreviewTerrainTypes.h"

#include "UTKTerrainPreviewComponent.generated.h"

struct FUTKTerrain;

UCLASS(ClassGroup=(UTK), meta=(BlueprintSpawnableComponent))
class UTK_API UUTKTerrainPreviewComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UUTKTerrainPreviewComponent();

	void ClearPreview();

	void UpdateFromTerrain(
		const FUTKTerrain& Terrain,
		FName LayerName,
		const FUTKPreviewTerrainMapping& Mapping);

	void UpdateFlatPreview(const FUTKPreviewTerrainMapping& Mapping);

	bool HasValidPreview() const { return bHasValidPreview; }

	FName GetCurrentLayerName() const { return CurrentLayerName; }
	const FUTKPreviewTerrainMapping& GetCurrentMapping() const { return CurrentMapping; }

	EUTKPreviewBackend GetActiveBackendType() const { return ActiveBackendType; }
	FBoxSphereBounds GetPreviewBounds() const;

private:
	void ClearRenderBackend();
	bool UpdateDynamicMeshBackend(const FUTKTerrain& Terrain, FName LayerName, const FUTKPreviewTerrainMapping& Mapping);
	bool UpdateFlatDynamicMeshBeckend(const FUTKPreviewTerrainMapping& Mapping);
	USceneComponent* EnsureDynamicMeshRenderComponent();

private:
	bool bHasValidPreview = false;

	FName CurrentLayerName = NAME_None;
	FUTKPreviewTerrainMapping CurrentMapping;

	EUTKPreviewBackend ActiveBackendType = EUTKPreviewBackend::None;

	UPROPERTY(Transient)
	TObjectPtr<USceneComponent> ActiveRenderComponent = nullptr;
};