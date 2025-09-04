#include "AssetTools/UTKAssetFactory.h"
#include "Assets/UTKAsset.h"
#include "AssetToolsModule.h"
#include "Graph/UTKGraph.h"
#include "Graph/UTKGraphSchema.h"

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

	UUTKAsset* Asset = NewObject<UUTKAsset>(InParent, Class, Name, Flags);

	if (!Asset->Graph)
	{
		UUTKGraph* Graph = NewObject<UUTKGraph>(Asset, TEXT("Graph"), RF_Transactional);
		Graph->Schema = UUTKGraphSchema::StaticClass();
		Asset->Graph = Graph;
	}

	return Asset;
}

uint32 UUTKAssetFactory::GetMenuCategories() const
{
	return EAssetTypeCategories::Misc;
}

bool UUTKAssetFactory::ShouldShowInNewMenu() const
{
	return true;
}