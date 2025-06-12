#include "AssetTools/AssetTypeActions_UTKAsset.h"
#include "Assets/UTKAsset.h"
#include "Toolkit/UTKEditorApp.h"

void FAssetTypeActions_UTKAsset::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	for (UObject* Obj : InObjects)
	{
		if (UUTKAsset* Asset = Cast<UUTKAsset>(Obj))
		{
			TSharedRef<FUTKEditorApp> Editor = MakeShareable(new FUTKEditorApp());
			Editor->InitUTKEditor(EToolkitMode::Standalone, EditWithinLevelEditor, Asset);
		}
	}
}