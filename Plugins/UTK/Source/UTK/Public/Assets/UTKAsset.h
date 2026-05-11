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

	// ----------------------------------------------------------
	// Preview Settings
	// ----------------------------------------------------------

	/**
	 * Preview heightfield resolution.
	 * This controls the number of samples used for editor preview evaluation.
	 */
	UPROPERTY(EditAnywhere, Category="Preview", meta=(ClampMin="16", UIMin="16"))
	int32 PreviewResolution = 512;

	/**
	 * Preview random seed used by procedural nodes.
	 */
	UPROPERTY(EditAnywhere, Category="Preview")
	int32 PreviewSeed = 0;

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