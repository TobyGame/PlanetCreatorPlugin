#include "AssetTools/AssetTypeActions_TerrainAsset.h"
#include "Assets/TerrainAsset.h"
#include "Toolkit/TerrainEditor.h"

void FAssetTypeActions_TerrainAsset::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	for (UObject* Obj : InObjects)
	{
		if (UTerrainAsset* Asset = Cast<UTerrainAsset>(Obj))
		{
			TSharedRef<FTerrainEditor> Editor = MakeShareable(new FTerrainEditor());
			Editor->InitTerrainEditor(EToolkitMode::Standalone, EditWithinLevelEditor, Asset);
		}
	}
}