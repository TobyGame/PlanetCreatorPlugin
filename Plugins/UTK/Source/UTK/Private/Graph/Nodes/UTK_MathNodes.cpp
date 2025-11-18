#include "Graph/Nodes/UTK_MathNodes.h"
#include "Graph/Nodes/UTKNodeExecutionTypes.h"
#include "Graph/Nodes/UTKNode.h"
#include "Graph/Nodes/UTKNodeDefinition.h"
#include "Graph/Nodes/UTKNodeFactory.h"
#include "Graph/Nodes/UTKNodeMacros.h"
#include "Graph/Nodes/NodeProperties/UTKPropertyTypes.h"
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
	{
	DEFINE_PIN("Result", false, true)
	},
	{
	DEFINE_PROPERTY("Value", FUTKFloatProperty{ 1.0f }, "Settings")
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

		Outputs.SetNum(1);

		const float Fill = Node.GetProperty<FUTKFloatProperty>("Value").Value;

		FUTKDomain2D Domain(Ctx.ResolutionX, Ctx.ResolutionY);
		TSharedPtr<FUTKTerrain> Terrain = MakeShared<FUTKTerrain>(Domain);
		FUTKLayer& HeightLayer = Terrain->GetOrCreateLayer(FName(TEXT("Height")));

		TSharedPtr<FUTKBuffer2D> Buffer = HeightLayer.Data;
		if (!Buffer.IsValid())
		{
		Node.AccessDiagnostics().SetMessage(TEXT("Failed to create height buffer."), true);
		return;
		}

		for (int32 Y = 0; Y < Domain.Height; ++Y)
		{
		for (int32 X = 0; X < Domain.Width; ++X)
		{
		Buffer->Set(X, Y, Fill);
		}
		}

		FUTKNodeOutput& Out = Outputs[0];
		Out.Terrain = Terrain;
		Out.DefaultLayerName = FName(TEXT("Height"));

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
	{
	DEFINE_PIN("A", true, true)
	DEFINE_PIN("B", true, true)
	DEFINE_PIN("Out", false, true)
	},
	{
	DEFINE_PROPERTY("Ratio", FUTKFloatProperty{ 0.5f }, "Settings")
	DEFINE_PROPERTY("SwapInputs", FUTKBoolProperty{ false }, "Settings")
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

		const float Ratio = Node.GetProperty<FUTKFloatProperty>("Ratio").Value;
		const bool bSwap = Node.GetProperty<FUTKBoolProperty>("SwapInputs").Value;

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
		Node.AccessDiagnostics().SetMessage(TEXT("Failed to create output buffer."), true);
		return;
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

		FUTKNodeOutput& Out = Outputs[0];
		Out.Terrain = OutTerrain;
		Out.DefaultLayerName = FName(TEXT("Height"));

		Node.AccessDiagnostics().SetMessage(TEXT("OK"), false);
		})
);

void RegisterMathNodes()
{
	REGISTER_UTK_NODE(Constant);
	REGISTER_UTK_NODE(Combine);
}