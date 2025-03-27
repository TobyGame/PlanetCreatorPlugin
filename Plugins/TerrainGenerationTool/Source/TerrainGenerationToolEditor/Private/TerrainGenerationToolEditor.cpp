#include "TerrainGenerationToolEditor.h"
#include "Modules/ModuleManager.h"
#include "Tools/AssetTypeActions_TerrainAsset.h"
#include "AssetToolsModule.h"

#define LOCTEXT_NAMESPACE "FTerrainGenerationToolEditorModule"

void FTerrainGenerationToolEditorModule::StartupModule()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	AssetTypeActions = MakeShareable(new FAssetTypeActions_TerrainAsset());
	AssetTools.RegisterAssetTypeActions(AssetTypeActions.ToSharedRef());
}

void FTerrainGenerationToolEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools") && AssetTypeActions.IsValid())
	{
		FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
		AssetToolsModule.Get().UnregisterAssetTypeActions(AssetTypeActions.ToSharedRef());
	}
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FTerrainGenerationToolEditorModule, TerrainGenerationToolEditor)