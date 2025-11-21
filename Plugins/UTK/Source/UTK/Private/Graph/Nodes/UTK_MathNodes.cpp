#include "Graph/Nodes/UTK_MathNodes.h"

#include "Graph/Nodes/UTKNodeExecutionTypes.h"
#include "Graph/Nodes/UTKNode.h"
#include "Graph/Nodes/UTKNodeDefinition.h"
#include "Graph/Nodes/UTKNodeFactory.h"
#include "Graph/Nodes/UTKNodeMacros.h"
#include "Core/UTKTerrainTypes.h"


/**
 * Constant node
 * - No inputs
 * - One output (Result) containing a Height layer filled with a constant value.
 */
DECLARE_UTK_NODE(
	Constant,
	"Constant",
	"Math",
	UUTKConstantSettings::StaticClass(),
	{
	DEFINE_PIN("Result", false, true)
	},
	([](const TArray<FUTKNodeInput>& Inputs,
			TArray<FUTKNodeOutput>& Outputs,
			FUTKNodeExecutionContext& Ctx,
			FUTKTerrainWorkspace& Workspace,
			UUTKNode& Node)
		{
		if (!Ctx.IsValid())
		{
		Node.AccessDiagnostics().SetMessage(TEXT("Invalid resolution."), true);
		return;
		}

		const UUTKConstantSettings* Settings = Node.GetSettingsTyped<UUTKConstantSettings>();
		const float ConstantValue = Settings ? Settings->Value : 0.0f;

		Outputs.SetNum(1);

		FUTKDomain2D Domain(Ctx.ResolutionX, Ctx.ResolutionY);
		TSharedPtr<FUTKTerrain> Terrain = MakeShared<FUTKTerrain>(Domain);
		FUTKLayer& HeightLayer = Terrain->GetOrCreateLayer(FName(TEXT("Height")));

		TSharedPtr<FUTKBuffer2D> Buffer = HeightLayer.Data;
		if (!Buffer.IsValid())
		{
		Buffer = MakeShared<FUTKBuffer2D>();
		Buffer->Initialize(Domain.Width, Domain.Height);
		HeightLayer.Data = Buffer.ToSharedRef();
		}

		for (int32 Y = 0; Y < Domain.Height; ++Y)
		{
		for (int32 X = 0; X < Domain.Width; ++X)
		{
		Buffer->Set(X, Y, ConstantValue);
		}
		}

		Outputs[0].Terrain = Terrain;
		Outputs[0].DefaultLayerName = TEXT("Height");

		Node.AccessDiagnostics().SetMessage(TEXT("OK"), false);
		})
);

/**
 * Combine node
 * - Two inputs (A, B), one output (Out) on Height layer.
 * - Simple linear blend controlled by Ratio, with optional input swap.
 */
DECLARE_UTK_NODE(
	Combine,
	"Combine",
	"Math",
	UUTKCombineSettings::StaticClass(),
	{
	DEFINE_PIN("A", true, true)
	DEFINE_PIN("B", true, true)
	DEFINE_PIN("Out", false, true)
	},
	([](const TArray<FUTKNodeInput>& Inputs,
			TArray<FUTKNodeOutput>& Outputs,
			FUTKNodeExecutionContext& Ctx,
			FUTKTerrainWorkspace& Workspace,
			UUTKNode& Node)
		{
		if (!Ctx.IsValid())
		{
		Node.AccessDiagnostics().SetMessage(TEXT("Invalid resolution."), true);
		return;
		}

		if (Inputs.Num() < 2 || !Inputs[0].HasTerrain() || !Inputs[1].HasTerrain())
		{
		Node.AccessDiagnostics().SetMessage(TEXT("Missing inputs."), true);
		return;
		}

		Outputs.SetNum(1);

		const UUTKCombineSettings* Settings = Node.GetSettingsTyped<UUTKCombineSettings>();
		const float Ratio = Settings ? Settings->Ratio : 0.5f;
		const bool bSwap = Settings ? Settings->bSwapInputs : false;

		const FUTKNodeInput& InA = bSwap ? Inputs[1] : Inputs[0];
		const FUTKNodeInput& InB = bSwap ? Inputs[0] : Inputs[1];

		const FUTKLayer* LayerA = InA.FindDefaultLayer();
		const FUTKLayer* LayerB = InB.FindDefaultLayer();

		if (!LayerA || !LayerB)
		{
		Node.AccessDiagnostics().SetMessage(TEXT("Invalid input layers."), true);
		return;
		}

		const FUTKBuffer2D& BufA = *LayerA->Data;
		const FUTKBuffer2D& BufB = *LayerB->Data;

		if (BufA.Width != Ctx.ResolutionX || BufA.Height != Ctx.ResolutionY ||
			BufB.Width != Ctx.ResolutionX || BufB.Height != Ctx.ResolutionY)
		{
		Node.AccessDiagnostics().SetMessage(TEXT("Input resolutions mismatch."), true);
		return;
		}

		FUTKDomain2D Domain(Ctx.ResolutionX, Ctx.ResolutionY);
		TSharedPtr<FUTKTerrain> OutTerrain = MakeShared<FUTKTerrain>(Domain);
		FUTKLayer& OutHeight = OutTerrain->GetOrCreateLayer(FName(TEXT("Height")));

		TSharedPtr<FUTKBuffer2D> OutBuffer = OutHeight.Data;
		if (!OutBuffer.IsValid())
		{
		OutBuffer = MakeShared<FUTKBuffer2D>();
		OutBuffer->Initialize(Domain.Width, Domain.Height);
		OutHeight.Data = OutBuffer.ToSharedRef();
		}

		for (int32 Y = 0; Y < Domain.Height; ++Y)
		{
		for (int32 X = 0; X < Domain.Width; ++X)
		{
		const float Aval = BufA.Get(X, Y);
		const float Bval = BufB.Get(X, Y);
		const float Value = FMath::Lerp(Aval, Bval, Ratio);
		OutBuffer->Set(X, Y, Value);
		}
		}

		Outputs[0].Terrain = OutTerrain;
		Outputs[0].DefaultLayerName = TEXT("Height");

		Node.AccessDiagnostics().SetMessage(TEXT("OK"), false);
		})
);

void RegisterMathNodes()
{
	REGISTER_UTK_NODE(Constant);
	REGISTER_UTK_NODE(Combine);
}