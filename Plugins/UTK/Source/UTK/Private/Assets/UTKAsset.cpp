#include "Assets/UTKAsset.h"

UUTKAsset::UUTKAsset()
{
	RefreshPreviewDerivedValues();
}

float UUTKAsset::GetPreviewMetersPerPixel() const
{
	return PreviewResolution > 0
		? PreviewWidthMeters / static_cast<float>(PreviewResolution)
		: 0.0f;
}

float UUTKAsset::GetPreviewHeightScaleRatio() const
{
	return PreviewWidthMeters > 0.0f
		? PreviewMaxHeightMeters / PreviewWidthMeters
		: 0.0f;
}

void UUTKAsset::RefreshPreviewDerivedValues()
{
	PreviewMetersPerPixel = GetPreviewMetersPerPixel();
	PreviewHeightScaleRatio = GetPreviewHeightScaleRatio();
}

void UUTKAsset::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(RF_ClassDefaultObject))
		RefreshPreviewDerivedValues();
}

void UUTKAsset::PostLoad()
{
	Super::PostLoad();

	RefreshPreviewDerivedValues();
}

#if WITH_EDITOR
void UUTKAsset::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	RefreshPreviewDerivedValues();
}
#endif