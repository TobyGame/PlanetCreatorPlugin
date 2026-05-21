#include "Assets/UTKAsset.h"

UUTKAsset::UUTKAsset()
{
	RefreshPreviewDerivedValues();
}

bool UUTKAsset::IsSupportedPreviewResolution(int32 InResolution)
{
	return InResolution == 512 ||
		InResolution == 1024 ||
		InResolution == 2048 ||
		InResolution == 4096;
}

int32 UUTKAsset::NormalizePreviewResolution(int32 InResolution)
{
	if (InResolution <= 512)
		return 512;

	if (InResolution <= 1024)
	{
		const int32 DistanceTo512 = FMath::Abs(InResolution - 512);
		const int32 DistanceTo1024 = FMath::Abs(InResolution - 1024);
		return DistanceTo512 <= DistanceTo1024 ? 512 : 1024;
	}

	if (InResolution <= 2048)
	{
		const int32 DistanceTo1024 = FMath::Abs(InResolution - 1024);
		const int32 DistanceTo2048 = FMath::Abs(InResolution - 2048);
		return DistanceTo1024 <= DistanceTo2048 ? 1024 : 2048;
	}

	if (InResolution <= 4096)
	{
		const int32 DistanceTo2048 = FMath::Abs(InResolution - 2048);
		const int32 DistanceTo4096 = FMath::Abs(InResolution - 4096);
		return DistanceTo2048 <= DistanceTo4096 ? 2048 : 4096;
	}

	return 4096;
}

float UUTKAsset::GetPreviewMetersPerPixel() const
{
	const int32 EffectiveResolution = NormalizePreviewResolution(PreviewResolution);

	return EffectiveResolution > 0
		? PreviewWidthMeters / static_cast<float>(EffectiveResolution)
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
	PreviewResolution = NormalizePreviewResolution(PreviewResolution);

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