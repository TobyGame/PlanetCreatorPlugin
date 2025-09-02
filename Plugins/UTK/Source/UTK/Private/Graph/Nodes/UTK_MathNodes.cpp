#include "Graph/Nodes/UTK_MathNodes.h"
#include "Core/UTKDataSystem.h"
#include "Graph/Nodes/UTKNode.h"
#include "Graph/Nodes/UTKNodeDefinition.h"
#include "Graph/Nodes/UTKNodeFactory.h"
#include "Graph/Nodes/UTKNodeMacros.h"
#include "Graph/Nodes/NodeProperties/UTKPropertyTypes.h"


DECLARE_UTK_NODE(
	Constant,
	"Constant",
	"Math",
	{
	DEFINE_PIN("Result",false, true)
	},
	{
	DEFINE_PROPERTY("Value", FUTKFloatProperty{1.0f}, "Settings")
	},
	([](FUTKDataSet& DataSet, UUTKNode* Node, int32 ResolutionX, int32 ResolutionY){
		const float Fill = Node->GetProperty<FUTKFloatProperty>("Value").Value;

		TSharedPtr<FTerrain2D> Height = DataSet.GetOrCreate("Height", ResolutionX, ResolutionY);

		for (int32 Y = 0; Y < ResolutionY; ++Y)
		{
		for (int32 X = 0; X < ResolutionX; ++X)
		{
		Height->Set(X, Y, Fill);
		}
		}
		})
);

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
	([](FUTKDataSet DataSet, UUTKNode* Node, int32 ResolutionX, int32 ResolutionY)
		{
		const float Ratio = Node->GetProperty<FUTKFloatProperty>("Ratio").Value;
		const bool bSwap = Node->GetProperty<FUTKBoolProperty>("SwapInputs").Value;

		TSharedPtr<FTerrain2D> A = DataSet.Get(bSwap ? "B" : "A");
		TSharedPtr<FTerrain2D> B = DataSet.Get(bSwap ? "A" : "B");

		if (!A || !B)
		{
		Node->AccessDiagnostics().SetMessage(TEXT("Missing inputs."), true);
		return;
		}

		TSharedPtr<FTerrain2D> Out = DataSet.GetOrCreate("Height", ResolutionX, ResolutionY);

		for (int32 Y = 0; Y < ResolutionY; ++Y)
		{
		for (int32 X = 0; X < ResolutionX; ++X)
		{
		const float Aval = A->Get(X, Y);
		const float Bval = B->Get(X, Y);
		Out->Set(X, Y, FMath::Lerp(Aval, Bval, Ratio));
		}
		}

		Node->AccessDiagnostics().SetMessage(TEXT("OK"), false);
		})
);




void RegisterMathNodes()
{
	REGISTER_UTK_NODE(Constant)
	REGISTER_UTK_NODE(Combine)
}