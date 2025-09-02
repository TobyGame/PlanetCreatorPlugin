#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "Assets/UTKAsset.h"

class FAssetTypeActions_UTKAsset : public FAssetTypeActions_Base
{
public:
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "FAssetTypeActions_UTKAsset", "UTK Asset"); }
	virtual FColor GetTypeColor() const override { return FColor(100, 200, 255); }
	virtual UClass* GetSupportedClass() const override { return UUTKAsset::StaticClass(); }
	virtual uint32 GetCategories() override { return EAssetTypeCategories::Misc; }

	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor) override;
};