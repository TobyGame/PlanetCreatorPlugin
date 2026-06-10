#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Preview/UTKPreviewTerrainTypes.h"

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

	// ----------------------------------------------------------
	// Preview Settings
	// ----------------------------------------------------------

	/**
	 * Preview heightfield resolution.
	 * This controls the number of samples used for editor preview evaluation.
	 */
	UPROPERTY(EditAnywhere, Category="Preview", meta=(ClampMin="512", ClampMax="4096", UIMin="512", UIMax="4096", Step="512"))
	int32 PreviewResolution = 512;

	/**
	 * Preview random seed used by procedural nodes.
	 */
	UPROPERTY(EditAnywhere, Category="Preview")
	int32 PreviewSeed = 0;

	/**
	 * Preferred viewport preview backend.
	 *
	 * Dynamic Mesh is kept as a CPU/debug backend.
	 * Nanite Height Texture uses UTK's built-in plugin preview mesh and material.
	 * Chunked Nanite Height Texture is reserved for future large-region preview.
	 */
	UPROPERTY(EditAnywhere, Category="Preview|Rendering", meta=(DisplayName="Preview Backend"))
	EUTKPreviewBackend PreviewBackend = EUTKPreviewBackend::DynamicMesh;

	/**
	 * Debug switch for testing GPU heightfield generation.
	 * 
	 * When enabled, Nanite Height Texture mode receives a GPU-generated render target
	 * instead of a CPU-uploaded height texture.
	 */
	UPROPERTY(EditAnywhere, Category="Preview|GPU Test", meta=(DisplayName="Enable GPU Height Test"))
	bool bPreviewUseGpuHeightTets = false;

	UPROPERTY(EditAnywhere, Category="Preview|GPU Test", meta=(ClampMin="0.1", UIMin="0.1", DisplayName="GPU Test Frequency"))
	float PreviewGpuHeightTestFrequency = 8.0f;

	UPROPERTY(EditAnywhere, Category="Preview|GPU Test", meta=(ClampMin="0.1", UIMin="0.1", UIMax="2.0", DisplayName="GPU Test Radius"))
	float PreviewGpuHeightTestRadius = 1.0f;

	UPROPERTY(EditAnywhere, Category="Preview|GPU Test", meta=(DisplayName="GPU Test Phase"))
	float PreviewGpuHeightTestPhase = 0.0f;

	/**
	 * Conceptual terrain width in meters.
	 * 
	 * This does not directly resize the 3D viewport footprint.
	 * Instead, it contributes to the terrain steepness ratio:
	 * 
	 * Height-Scale Ratio = PreviewMaxHeightMeters / PreviewWidthMeters
	 */
	UPROPERTY(EditAnywhere, Category="Preview|Terrain Definition", meta=(ClampMin="1.0", UIMin="1.0", DisplayName="Width (m)"))
	float PreviewWidthMeters = 5000.0f;

	/**
	 * Conceptual maximum terrain height in meters.
	 * 
	 * Increasing this value makes the 3D terrain preview appear taller once
	 * heightfield mesh rendering is implemented.
	 */
	UPROPERTY(EditAnywhere, Category="Preview|Terrain Definition", meta=(ClampMin="1.0", UIMin="1.0", DisplayName="Height (m)"))
	float PreviewMaxHeightMeters = 2500.0f;

	/**
	 * Derived read-only value.
	 * 
	 * Real horizontal scale represented by one preview sample.
	 * 
	 * Compute as:
	 * PreviewWidthMeters / PreviewResolution
	 */
	UPROPERTY(VisibleAnywhere, Transient, Category="Preview|Terrain Definition", meta=(DisplayName="Real Scale (m/px)"))
	float PreviewMetersPerPixel = 0.0f;

	/**
	 * Derived read-only value.
	 * 
	 * This is the important value for terrain preview steepness.
	 * 
	 * Computed as:
	 * PreviewMaxHeightMeters / PreviewWidthMeters
	 */
	UPROPERTY(VisibleAnywhere, Transient, Category="Preview|Terrain Definition", meta=(DisplayName="Height-Scale Ratio"))
	float PreviewHeightScaleRatio = 0.0f;

	static bool IsSupportedPreviewResolution(int32 InResolution);
	static int32 NormalizePreviewResolution(int32 InResolution);

	float GetPreviewMetersPerPixel() const;
	float GetPreviewHeightScaleRatio() const;
	void RefreshPreviewDerivedValues();

	virtual void PostInitProperties() override;
	virtual void PostLoad() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UPROPERTY(Instanced)
	TObjectPtr<UUTKGraph> Graph = nullptr;
};