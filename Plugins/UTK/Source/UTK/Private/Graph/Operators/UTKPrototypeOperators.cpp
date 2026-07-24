#include "Graph/Operators/UTKPrototypeOperators.h"

#include "Core/UTKTerrainTypes.h"
#include "Graph/Nodes/UTK_MathNodes.h"
#include "Graph/Nodes/UTKNode.h"
#include "Graph/Operators/UTKOperatorRegistry.h"

namespace
{
	const FUTKLayer* ResolvePrimaryLayer(const FUTKNodeInput& Input)
	{
		if (const FUTKLayer* DefaultLayer = Input.FindDefaultLayer())
			return DefaultLayer;

		return Input.HasTerrain() ? Input.Terrain->FindAnyLayer() : nullptr;
	}

	FUTKBuffer2D* GetOutputBuffer(const TSharedPtr<FUTKTerrain>& Terrain, TArray<FUTKNodeOutput>& Outputs, int32 OutputIndex, FString& OutError)
	{
		if (!Terrain.IsValid() || !Outputs.IsValidIndex(OutputIndex))
		{
			OutError = TEXT("Unexpected output index.");
			return nullptr;
		}

		const FName LayerName = Outputs[OutputIndex].DefaultLayerName;

		if (LayerName.IsNone())
		{
			OutError = TEXT("Output layer name is missing.");
			return nullptr;
		}

		FUTKLayer& Layer = Terrain->GetOrCreateLayer(LayerName);

		Outputs[OutputIndex].Terrain = Terrain;

		return &Layer.Data.Get();
	}

	bool EvaluatePrototypeConstant(const FUTKOperatorReferenceEvaluationRequest& Request, TArray<FUTKNodeOutput>& InOutOutputs, FString& OutError)
	{
		if (!Request.Context.IsValid())
		{
			OutError = TEXT("Invalid evaluation resolution.");
			return false;
		}

		if (InOutOutputs.Num() != 1)
		{
			OutError = TEXT("Constant expects one output.");
			return false;
		}

		const UUTKConstantSettings* Settings = Request.Node.GetSettingsTyped<UUTKConstantSettings>();

		const float Value = FMath::Clamp(Settings ? Settings->Value : 0.0f, 0.0f, 1.0f);

		const FUTKDomain2D Domain(Request.Context.ResolutionX, Request.Context.ResolutionY);

		TSharedPtr<FUTKTerrain> Terrain = MakeShared<FUTKTerrain>(Domain);

		FUTKBuffer2D* Output = GetOutputBuffer(Terrain, InOutOutputs, 0, OutError);

		if (!Output)
			return false;

		Output->Fill(Value);
		return true;
	}

	bool EvaluatePrototypeCombine(const FUTKOperatorReferenceEvaluationRequest& Request, TArray<FUTKNodeOutput>& InOutOutputs, FString& OutError)
	{
		if (!Request.Context.IsValid())
		{
			OutError = TEXT("Invalid evaluation resolution.");
			return false;
		}

		if (Request.Inputs.Num() != 2 || InOutOutputs.Num() != 1)
		{
			OutError = TEXT("Combine expects two inputs and one output.");
			return false;
		}

		const UUTKCombineSettings* Settings = Request.Node.GetSettingsTyped<UUTKCombineSettings>();

		const float Ratio = FMath::Clamp(Settings ? Settings->Ratio : 0.5f, 0.0f, 1.0f);

		const bool bSwapInputs = Settings && Settings->bSwapInputs;

		const FUTKNodeInput& InputA = bSwapInputs ? Request.Inputs[1] : Request.Inputs[0];

		const FUTKNodeInput& InputB = bSwapInputs ? Request.Inputs[0] : Request.Inputs[1];

		const FUTKLayer* LayerA = ResolvePrimaryLayer(InputA);
		const FUTKLayer* LayerB = ResolvePrimaryLayer(InputB);

		if (!LayerA || !LayerB)
		{
			OutError = TEXT("Combine inputs layers are missing.");
			return false;
		}

		const FUTKBuffer2D& BufferA = LayerA->Data.Get();
		const FUTKBuffer2D& BufferB = LayerB->Data.Get();

		if (BufferA.Width != Request.Context.ResolutionX ||
			BufferA.Height != Request.Context.ResolutionY ||
			BufferB.Width != Request.Context.ResolutionX ||
			BufferB.Height != Request.Context.ResolutionY)
		{
			OutError = TEXT("Combine input resolution mismatch.");
			return false;
		}

		const FUTKDomain2D Domain(Request.Context.ResolutionX, Request.Context.ResolutionY);

		TSharedPtr<FUTKTerrain> Terrain = MakeShared<FUTKTerrain>(Domain);

		FUTKBuffer2D* Output = GetOutputBuffer(Terrain, InOutOutputs, 0, OutError);

		if (!Output)
			return false;

		for (int32 Y = 0; Y < Domain.Height; ++Y)
		{
			for (int32 X = 0; X < Domain.Width; ++X)
			{
				Output->Set(X, Y, FMath::Lerp(BufferA.Get(X, Y), BufferB.Get(X, Y), Ratio));
			}
		}

		return true;
	}

	bool EvaluatePrototypeMultiOutput(const FUTKOperatorReferenceEvaluationRequest& Request, TArray<FUTKNodeOutput>& InOutOutputs, FString& OutError)
	{
		if (!Request.Context.IsValid())
		{
			OutError = TEXT("Invalid evaluation resolution.");
			return false;
		}

		if (Request.Inputs.Num() != 1 || InOutOutputs.Num() != 3)
		{
			OutError = TEXT("Multi Output Test expects one input and three outputs.");
			return false;
		}

		const FUTKLayer* InputLayer = ResolvePrimaryLayer(Request.Inputs[0]);

		if (!InputLayer)
		{
			OutError = TEXT("Multi Output Test input layer is missing.");
			return false;
		}

		const FUTKBuffer2D& Input = InputLayer->Data.Get();

		if (Input.Width != Request.Context.ResolutionX ||
			Input.Height != Request.Context.ResolutionY)
		{
			OutError = TEXT("Multi Output Test input resolution mismatch.");
			return false;
		}

		const FUTKDomain2D Domain(Request.Context.ResolutionX, Request.Context.ResolutionY);

		TSharedPtr<FUTKTerrain> Terrain = MakeShared<FUTKTerrain>(Domain);

		FUTKBuffer2D* Base = GetOutputBuffer(Terrain, InOutOutputs, 0, OutError);
		FUTKBuffer2D* Low = GetOutputBuffer(Terrain, InOutOutputs, 1, OutError);
		FUTKBuffer2D* High = GetOutputBuffer(Terrain, InOutOutputs, 2, OutError);

		if (!Base || !Low || !High)
			return false;

		for (int32 Y = 0; Y < Domain.Height; ++Y)
		{
			for (int32 X = 0; X < Domain.Width; ++X)
			{
				const float Value = Input.Get(X, Y);

				Base->Set(X, Y, Value);
				Low->Set(X, Y, Value * 0.5f);
				High->Set(X, Y, FMath::Clamp(Value * Value, 0.0f, 1.0f));
			}
		}

		return true;
	}
}

void RegisterPrototypeOperators()
{
	FUTKOperatorRegistry& Registry = FUTKOperatorRegistry::Get();

	Registry.RegisterOperator(
		FUTKOperatorSpecBuilder(TEXT("UTK.Internal.Prototype.Constant"))
		.Family(EUTKKernelFamily::ProceduralGenerator)
		.Output(EUTKFieldType::Scalar)
		.Parameter(
			TEXT("Value"),
			GET_MEMBER_NAME_CHECKED(UUTKConstantSettings, Value),
			EUTKParameterType::Float)
		.PrototypeOnly()
		.ReferenceEvaluator(&EvaluatePrototypeConstant)
		.Build());

	Registry.RegisterOperator(
		FUTKOperatorSpecBuilder(TEXT("UTK.Internal.Prototype.Lerp"))
		.Family(EUTKKernelFamily::Pointwise)
		.Input(EUTKFieldType::Scalar)
		.Input(EUTKFieldType::Scalar)
		.Output(EUTKFieldType::Scalar)
		.Parameter(
			TEXT("Ratio"),
			GET_MEMBER_NAME_CHECKED(UUTKCombineSettings, Ratio),
			EUTKParameterType::Float)
		.Parameter(
			TEXT("SwapInputs"),
			GET_MEMBER_NAME_CHECKED(UUTKCombineSettings, bSwapInputs),
			EUTKParameterType::Bool)
		.PrototypeOnly()
		.SupportsPointwiseFusion()
		.ReferenceEvaluator(&EvaluatePrototypeCombine)
		.Build());

	Registry.RegisterOperator(
		FUTKOperatorSpecBuilder(TEXT("UTK.Internal.Prototype.MultiOutput"))
		.Family(EUTKKernelFamily::Pointwise)
		.Input(EUTKFieldType::Scalar)
		.Output(EUTKFieldType::Scalar)
		.Output(EUTKFieldType::Scalar)
		.Output(EUTKFieldType::Scalar)
		.PrototypeOnly()
		.ReferenceEvaluator(&EvaluatePrototypeMultiOutput)
		.Build());
}