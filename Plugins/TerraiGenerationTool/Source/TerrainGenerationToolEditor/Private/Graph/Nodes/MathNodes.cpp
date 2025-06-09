#include "Graph/Nodes/MathNodes.h"
#include "Core/TerrainDataSystem.h"
#include "Graph/Nodes/TerrainNode.h"
#include "Graph/Nodes/TerrainNodeDefinition.h"
#include "Graph/Nodes/TerrainNodeFactory.h"
#include "Graph/Nodes/TerrainNodeMacros.h"
#include "Graph/Nodes/NodeProperties/TerrainPropertyTypes.h"


DECLARE_TERRAIN_NODE(Constant,
	"Constant",
	"Math",
	{
	DEFINE_PIN("Result",false, true)
	},
	{
	DEFINE_PROPERTY("Value", FTerrainFloatProperty{1.0f}, "Settings")
	},
	([](FTerrainDataSet& DataSet, UTerrainNode* Node, int32 ResolutionX, int32 ResolutionY)
		{
		const float Fill = Node->GetProperty<FTerrainFloatProperty>("Value").Value;

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
	REGISTER_TERRAIN_NODE(Constant)
}