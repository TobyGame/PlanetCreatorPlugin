#include "Graph/Nodes/MathNodes.h"
#include "Graph/Nodes/TerrainNode.h"
#include "Graph/Nodes/TerrainNodeMacros.h"


DECLARE_TERRAIN_NODE(Constant,
	"Constant",
	"Math",
	{
	DEFINE_PIN("Result", Heightmap, false, true)
	},
	{
	DEFINE_PROPERTY("Value", FTerrainFloatProperty{1.0f}, "Settings")
	},
	([](const TArray<TSharedPtr<FTerrainData>>& Inputs, UTerrainNode* Node) -> TSharedPtr<FTerrainData>
		{
		const float Fill = Node->GetProperty<FTerrainFloatProperty>("Value").Value;

		TSharedPtr<FTerrainData> Out = MakeShared<FTerrainData>();
		Out->Heightmap = MakeShared<FTerrainHeightmap>();
		Out->Heightmap->Resize(512, 512);

		for (int32 Y = 0; Y < Out->Heightmap->Height; ++Y)
		{
		for (int32 X = 0; X < Out->Heightmap->Width; ++X)
		{
		Out->Heightmap->Set(X, Y, Fill);
		}
		}

		Out->SourceNodeName = Node->GetFName();
		return Out;
		})
);


void RegisterMathNodes()
{
	REGISTER_TERRAIN_NODE(Constant)
}