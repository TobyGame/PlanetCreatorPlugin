#include "AssetTools/TerrainAssetFactory.h"
#include "Assets/TerrainAsset.h"
#include "AssetToolsModule.h"

UTerrainAssetFactory::UTerrainAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UTerrainAsset::StaticClass();
}

UObject* UTerrainAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	if (Class == nullptr || !Class->IsChildOf(UTerrainAsset::StaticClass()))
	{
		return nullptr;
	}

	return NewObject<UTerrainAsset>(InParent, Class, Name, Flags);
}

uint32 UTerrainAssetFactory::GetMenuCategories() const
{
	return EAssetTypeCategories::Misc;
}

bool UTerrainAssetFactory::ShouldShowInNewMenu() const
{
	return true;
}