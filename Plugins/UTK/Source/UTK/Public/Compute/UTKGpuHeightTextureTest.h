#pragma once

#include "CoreMinimal.h"

class UTextureRenderTarget2D;

struct FUTKGpuHeightTextureTestSettings
{
	int32 Resolution = 512;

	float Frequency = 8.0f;
	float Radius = 1.0f;
	float Phase = 0.0f;
};

class FUTKGpuHeightTextureTest
{
public:
	static UTextureRenderTarget2D* CreateOrResizeRenderTarget(
		UObject* Outer,
		UTextureRenderTarget2D* ExistingRenderTarget,
		int32 Resolution);

	static void Dispatch(
		UTextureRenderTarget2D* RenderTarget,
		const FUTKGpuHeightTextureTestSettings& Settings);
};