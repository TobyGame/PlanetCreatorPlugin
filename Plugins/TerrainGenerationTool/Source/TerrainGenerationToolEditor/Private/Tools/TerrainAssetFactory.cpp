#include "Tools/TerrainAssetFactory.h"
#include "Asset/TerrainAsset.h"

UTerrainAssetFactory::UTerrainAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UTerrainAsset::StaticClass();
}

UObject* UTerrainAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UTerrainAsset>(InParent, InClass, InName, Flags);
}
