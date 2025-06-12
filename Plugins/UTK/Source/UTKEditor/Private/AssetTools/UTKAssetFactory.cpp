#include "AssetTools/UTKAssetFactory.h"
#include "Assets/UTKAsset.h"
#include "AssetToolsModule.h"

UUTKAssetFactory::UUTKAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UUTKAsset::StaticClass();
}

UObject* UUTKAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	if (Class == nullptr || !Class->IsChildOf(UUTKAsset::StaticClass()))
	{
		return nullptr;
	}

	return NewObject<UUTKAsset>(InParent, Class, Name, Flags);
}

uint32 UUTKAssetFactory::GetMenuCategories() const
{
	return EAssetTypeCategories::Misc;
}

bool UUTKAssetFactory::ShouldShowInNewMenu() const
{
	return true;
}