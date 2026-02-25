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


		if (Outputs.Num() == 0 || Outputs[0].DefaultLayerName.IsNone())
		{
		Node.AccessDiagnostics().SetMessage(TEXT("Constant: invalid output layer name."), true);
		return;
		}

		const FName OutputLayerName = Outputs[0].DefaultLayerName;

		FUTKDomain2D Domain(Ctx.ResolutionX, Ctx.ResolutionY);
		TSharedPtr<FUTKTerrain> Terrain = MakeShared<FUTKTerrain>(Domain);
		FUTKLayer& OutLayer = Terrain->GetOrCreateLayer(OutputLayerName);

		TSharedPtr<FUTKBuffer2D> Buffer = OutLayer.Data;
		if (!Buffer.IsValid())
		{
		Buffer = MakeShared<FUTKBuffer2D>();
		Buffer->Initialize(Domain.Width, Domain.Height);
		OutLayer.Data = Buffer.ToSharedRef();
		}

		for (int32 Y = 0; Y < Domain.Height; ++Y)
		{
		for (int32 X = 0; X < Domain.Width; ++X)
		{
		Buffer->Set(X, Y, ConstantValue);
		}
		}

		Outputs[0].Terrain = Terrain;

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


		if (Outputs.Num() == 0 || Outputs[0].DefaultLayerName.IsNone())
		{
		Node.AccessDiagnostics().SetMessage(TEXT("Combine: Invalid output layer name."), true);
		return;
		}

		const FName OutputLayerName = Outputs[0].DefaultLayerName;

		const UUTKCombineSettings* Settings = Node.GetSettingsTyped<UUTKCombineSettings>();
		const float Ratio = Settings ? Settings->Ratio : 0.5f;
		const bool bSwap = Settings ? Settings->bSwapInputs : false;

		const FUTKNodeInput& InA = bSwap ? Inputs[1] : Inputs[0];
		const FUTKNodeInput& InB = bSwap ? Inputs[0] : Inputs[1];

		const FUTKLayer* LayerA = InA.FindDefaultLayer();
		if (!LayerA && InA.HasTerrain())
		{
		LayerA = InA.Terrain->FindAnyLayer();
		}

		const FUTKLayer* LayerB = InB.FindDefaultLayer();
		if (!LayerB && InB.HasTerrain())
		{
		LayerB = InB.Terrain->FindAnyLayer();
		}


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
		FUTKLayer& OutLayer = OutTerrain->GetOrCreateLayer(OutputLayerName);

		TSharedPtr<FUTKBuffer2D> OutBuffer = OutLayer.Data;
		if (!OutBuffer.IsValid())
		{
		OutBuffer = MakeShared<FUTKBuffer2D>();
		OutBuffer->Initialize(Domain.Width, Domain.Height);
		OutLayer.Data = OutBuffer.ToSharedRef();
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

		Node.AccessDiagnostics().SetMessage(TEXT("OK"), false);
		})
);

DECLARE_UTK_NODE(
	MultiOutputTest,
	"Multi Output Test",
	"Debug",
	UUTKMultiOutputTestSettings::StaticClass(),
	{
	DEFINE_PIN("In", true, true)
	DEFINE_PIN("Base", false, true)
	DEFINE_PIN("Low", false, false)
	DEFINE_PIN("High", false, false)
	},
	([](const TArray<FUTKNodeInput>& Inputs,
			TArray<FUTKNodeOutput>& Outputs,
			FUTKNodeExecutionContext& Ctx,
			FUTKTerrainWorkspace& Workspace,
			UUTKNode& Node){
		if (!Ctx.IsValid())
		{
		Node.AccessDiagnostics().SetMessage(TEXT("Invalid resolution."), true);
		return;
		}

		if (Inputs.Num() < 1 || !Inputs[0].HasTerrain())
		{
		Node.AccessDiagnostics().SetMessage(TEXT("Missing input."), true);
		return;
		}

		const FUTKNodeInput& In = Inputs[0];

		const FUTKLayer* InLayer = In.FindDefaultLayer();
		if (!InLayer && In.HasTerrain())
		InLayer = In.Terrain->FindAnyLayer();

		if (!InLayer)
		{
		Node.AccessDiagnostics().SetMessage(TEXT("Invalid input layer."), true);
		return;
		}

		const FUTKBuffer2D& InBuffer = *InLayer->Data;

		if (InBuffer.Width != Ctx.ResolutionX ||
			InBuffer.Height != Ctx.ResolutionY)
		{
		Node.AccessDiagnostics().SetMessage(TEXT("Input resolution mismatch"), true);
		return;
		}

		const int32 Width = Ctx.ResolutionX;
		const int32 Height = Ctx.ResolutionY;

		FUTKDomain2D Domain(Width, Height);
		TSharedPtr<FUTKTerrain> OutTerrain = MakeShared<FUTKTerrain>(Domain);

		if (Outputs.Num() < 3)
		{
		Node.AccessDiagnostics().SetMessage(TEXT("MultiOutputTest: unexpected output count"), true);
		return;
		}

		auto PrepareLayer = [&Outputs, &OutTerrain, Width, Height](int32 Index) -> TSharedPtr<FUTKBuffer2D>{
		if (!Outputs.IsValidIndex(Index))
		return nullptr;

		const FName LayerName = Outputs[Index].DefaultLayerName;
		if (LayerName.IsNone())
		return nullptr;

		FUTKLayer& Layer = OutTerrain->GetOrCreateLayer(LayerName);

		TSharedPtr<FUTKBuffer2D> Buffer = Layer.Data;
		if (!Buffer.IsValid())
		{
		Buffer = MakeShared<FUTKBuffer2D>();
		Buffer->Initialize(Width, Height);
		Layer.Data = Buffer.ToSharedRef();
		}

		Outputs[Index].Terrain = OutTerrain;

		return Buffer;
		};

		TSharedPtr<FUTKBuffer2D> BaseBuffer = PrepareLayer(0);
		TSharedPtr<FUTKBuffer2D> LowBuffer = PrepareLayer(1);
		TSharedPtr<FUTKBuffer2D> HighBuffer = PrepareLayer(2);

		if (!BaseBuffer.IsValid())
		{
		Node.AccessDiagnostics().SetMessage(TEXT("MultiOutputTest: invalid Base output"), true);
		return;
		}

		for (int32 Y = 0; Y < Height; ++Y)
		{
		for (int32 X = 0; X < Width; ++X)
		{
		const float Value = InBuffer.Get(X, Y);

		BaseBuffer->Set(X, Y, Value);

		if (LowBuffer.IsValid())
		LowBuffer->Set(X, Y, Value * 0.5f);

		if (HighBuffer.IsValid())
		{
		const float HighVal = FMath::Clamp(Value * Value, 0.0f, 1.0f);
		HighBuffer->Set(X, Y, HighVal);
		}
		}
		}

		Node.AccessDiagnostics().SetMessage(TEXT("OK"), false);
		})
);

void RegisterMathNodes()
{
	REGISTER_UTK_NODE(Constant);
	REGISTER_UTK_NODE(Combine);
	REGISTER_UTK_NODE(MultiOutputTest);
}