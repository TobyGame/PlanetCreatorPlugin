#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Preview/UTKPreviewTerrainTypes.h"

#include "UTKTerrainPreviewComponent.generated.h"

class UDynamicMeshComponent;
class UMaterialInstanceConstant;
class UMaterialInterface;
class UStaticMesh;
class UStaticMeshComponent;
class UTexture2D;
class UTextureRenderTarget2D;

struct FUTKTerrain;
struct FUTKLayer;

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
	EUTKPreviewBackend ResolveSupportedBackend(const FUTKPreviewTerrainMapping& Mapping) const;
	bool UpdateRenderBackend(const FUTKTerrain& Terrain, FName LayerName, const FUTKPreviewTerrainMapping& Mapping);
	bool UpdateFlatRenderBackend(const FUTKPreviewTerrainMapping& Mapping);
	bool UpdateDynamicMeshBackend(const FUTKTerrain& Terrain, FName LayerName, const FUTKPreviewTerrainMapping& Mapping);
	bool UpdateFlatDynamicMeshBackend(const FUTKPreviewTerrainMapping& Mapping);
	bool UpdateNaniteHeightTextureBackend(const FUTKTerrain& Terrain, FName LayerName, const FUTKPreviewTerrainMapping& Mapping);
	bool UpdateFlatNaniteHeightTextureBackend(const FUTKPreviewTerrainMapping& Mapping);

	bool UpdateNanitePreviewMesh(const FUTKPreviewTerrainMapping& Mapping);
	bool UpdateHeightTextureFromLayer(const FUTKLayer& Layer, const FUTKPreviewTerrainMapping& Mapping);
	bool UpdateFlatHeightTexture(const FUTKPreviewTerrainMapping& Mapping);
	bool UpdateGpuHeightTextureTest(const FUTKPreviewTerrainMapping& Mapping);
	bool ApplyNaniteDisplacementMaterial(const FUTKPreviewTerrainMapping& Mapping);
	void ApplyNaniteBoundsScale(float MagnitudeUU);

	UTexture2D* CreateOrResizeHeightTexture(int32 Width, int32 Height);

	UDynamicMeshComponent* EnsureDynamicMeshRenderComponent();
	UStaticMeshComponent* EnsureNaniteStaticMeshComponent();

	void HideDynamicMeshBackend();
	void HideNaniteBackend();

private:
	bool bHasValidPreview = false;

	FName CurrentLayerName = NAME_None;
	FUTKPreviewTerrainMapping CurrentMapping;

	EUTKPreviewBackend ActiveBackendType = EUTKPreviewBackend::None;

	UPROPERTY(Transient)
	TObjectPtr<USceneComponent> ActiveRenderComponent = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UDynamicMeshComponent> DynamicMeshComponent = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UStaticMeshComponent> NaniteStaticMeshComponent = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> HeightTexture = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceConstant> NanitePreviewMaterialInstance = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInterface> NaniteDisplacementParentMaterial = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTexture> ActiveHeightTexture = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTexture> LastAppliedHeightTexture = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTextureRenderTarget2D> GpuHeightTestRenderTarget = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UStaticMesh> CurrentNanitePreviewMesh = nullptr;

	int32 HeightTextureWidth = 0;
	int32 HeightTextureHeight = 0;
	float LastAppliedNaniteMagnitudeUU = -1.0f;
};