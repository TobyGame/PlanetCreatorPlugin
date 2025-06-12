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
	([](FUTKDataSet& DataSet, UUTKNode* Node, int32 ResolutionX, int32 ResolutionY)
		{
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


void RegisterMathNodes()
{
	REGISTER_UTK_NODE(Constant)
}