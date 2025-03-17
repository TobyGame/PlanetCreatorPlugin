#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "Assets/TerrainAsset.h"

class FAssetTypeActions_TerrainAsset : public FAssetTypeActions_Base
{
public:
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "FAssetTypeActions_TerrainAsset", "Terrain Asset"); }
	virtual FColor GetTypeColor() const override { return FColor(100, 200, 255); }
	virtual UClass* GetSupportedClass() const override { return UTerrainAsset::StaticClass(); }
	virtual uint32 GetCategories() override { return EAssetTypeCategories::Misc; }
};