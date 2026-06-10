#include "Compute/UTKGpuHeightTextureTest.h"

#include "Core/UTKLogger.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GlobalShader.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "RenderTargetPool.h"
#include "RHI.h"
#include "ShaderParameterStruct.h"

class FUTKGpuHeightTestCS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FUTKGpuHeightTestCS);
	SHADER_USE_PARAMETER_STRUCT(FUTKGpuHeightTestCS, FGlobalShader);

public:
	static constexpr int32 ThreadGroupSizeX = 8;
	static constexpr int32 ThreadGroupSizeY = 8;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
		SHADER_PARAMETER(FIntPoint, OutputSize)
		SHADER_PARAMETER(float, Frequency)
		SHADER_PARAMETER(float, Radius)
		SHADER_PARAMETER(float, Phase)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, OutputTexture)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
};

IMPLEMENT_GLOBAL_SHADER(
	FUTKGpuHeightTestCS,
	"/Plugin/UTK/Private/UTKGpuHeightTest.usf",
	"MainCS",
	SF_Compute);

UTextureRenderTarget2D* FUTKGpuHeightTextureTest::CreateOrResizeRenderTarget(UObject* Outer, UTextureRenderTarget2D* ExistingRenderTarget, int32 Resolution)
{
	const int32 SafeResolution = FMath::Clamp(Resolution, 2, 4096);

	if (ExistingRenderTarget && ExistingRenderTarget->SizeX == SafeResolution && ExistingRenderTarget->SizeY == SafeResolution)
	{
		return ExistingRenderTarget;
	}

	UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>(
		Outer,
		MakeUniqueObjectName(Outer, UTextureRenderTarget2D::StaticClass(), TEXT("UTKGpuHeightRenderTarget")),
		RF_Transient);

	if (!RenderTarget)
		return nullptr;

	RenderTarget->RenderTargetFormat = RTF_RGBA8;
	RenderTarget->ClearColor = FLinearColor::Black;
	RenderTarget->bAutoGenerateMips = false;
	RenderTarget->bCanCreateUAV = true;
	RenderTarget->SRGB = false;
	RenderTarget->Filter = TF_Bilinear;
	RenderTarget->AddressX = TA_Clamp;
	RenderTarget->AddressY = TA_Clamp;

	RenderTarget->InitCustomFormat(SafeResolution, SafeResolution, PF_B8G8R8A8, false);

	RenderTarget->UpdateResourceImmediate(true);

	return RenderTarget;
}

void FUTKGpuHeightTextureTest::Dispatch(UTextureRenderTarget2D* RenderTarget, const FUTKGpuHeightTextureTestSettings& Settings)
{
	if (!RenderTarget)
		return;

	FTextureRenderTargetResource* RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();

	if (!RenderTargetResource)
		return;

	const int32 Width = RenderTarget->SizeX;
	const int32 Height = RenderTarget->SizeY;

	const float Frequency = Settings.Frequency;
	const float Radius = Settings.Radius;
	const float Phase = Settings.Phase;

	ENQUEUE_RENDER_COMMAND(UTKGpuHeightTest)(
		[RenderTargetResource, Width, Height, Frequency, Radius, Phase](FRHICommandListImmediate& RHICmdList){
			FTextureRHIRef RenderTargetRHI = RenderTargetResource->GetRenderTargetTexture();

			if (!RenderTargetRHI.IsValid())
				return;

			FRDGBuilder GraphBuilder(RHICmdList);

			FRDGTextureRef OutputTexture = GraphBuilder.RegisterExternalTexture(
				CreateRenderTarget(RenderTargetRHI, TEXT("UTKGpuHeightTextureTestRT")));

			FUTKGpuHeightTestCS::FParameters* PassParameter = GraphBuilder.AllocParameters<FUTKGpuHeightTestCS::FParameters>();

			PassParameter->OutputSize = FIntPoint(Width, Height);
			PassParameter->Frequency = Frequency;
			PassParameter->Radius = Radius;
			PassParameter->Phase = Phase;
			PassParameter->OutputTexture = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(OutputTexture));

			TShaderMapRef<FUTKGpuHeightTestCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

			const FIntVector GroupCount = FComputeShaderUtils::GetGroupCount(
				FIntPoint(Width, Height),
				FIntPoint(FUTKGpuHeightTestCS::ThreadGroupSizeX, FUTKGpuHeightTestCS::ThreadGroupSizeY));

			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("UTK GPU Height Texture Test"),
				ComputeShader,
				PassParameter,
				GroupCount);

			GraphBuilder.Execute();
		});
}