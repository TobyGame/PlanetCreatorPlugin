#pragma once

#include "CoreMinimal.h"
#include "Core/UTKGraphSaveData.h"

class UUTKAsset;
class UUTKGraph;

class FUTKGraphBuilder
{
public:
	static void LoadFromAsset(UUTKAsset* Asset, UUTKGraph* Graph);
	static void SaveToAsset(UUTKGraph* Graph, UUTKAsset* Asset);
};