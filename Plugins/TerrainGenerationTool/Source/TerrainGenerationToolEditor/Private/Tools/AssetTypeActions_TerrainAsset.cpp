#include "Tools/AssetTypeActions_TerrainAsset.h"
#include "Asset/TerrainAsset.h"

UClass* FAssetTypeActions_TerrainAsset::GetSupportedClass() const
{
	return UTerrainAsset::StaticClass();
}