#include "TerrainGenerationToolEditor.h"
#include "AssetToolsModule.h"
#include "AssetTools/TerrainAssetFactory.h"
#include "AssetTools/AssetTypeActions_TerrainAsset.h"

#define LOCTEXT_NAMESPACE "FTerrainGenerationToolEditorModule"

void FTerrainGenerationToolEditorModule::StartupModule()
{
}

void FTerrainGenerationToolEditorModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FTerrainGenerationToolEditorModule, TerrainGenerationToolEditor)