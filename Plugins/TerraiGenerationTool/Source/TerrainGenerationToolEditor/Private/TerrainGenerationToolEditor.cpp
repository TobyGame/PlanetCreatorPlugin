#include "TerrainGenerationToolEditor.h"
#include "AssetToolsModule.h"
#include "AssetTools/TerrainAssetFactory.h"
#include "AssetTools/AssetTypeActions_TerrainAsset.h"
#include "Graph/Nodes/MathNodes.h"
#include "Graph/Nodes/TerrainNodeFactory.h"

#define LOCTEXT_NAMESPACE "FTerrainGenerationToolEditorModule"

void FTerrainGenerationToolEditorModule::StartupModule()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	TSharedPtr<IAssetTypeActions> TerrainAssetTypeActions = MakeShareable(new FAssetTypeActions_TerrainAsset());
	AssetTools.RegisterAssetTypeActions(TerrainAssetTypeActions.ToSharedRef());

	RegisterAssetTypeActions.Add(TerrainAssetTypeActions);

	RegisterMathNodes();
}

void FTerrainGenerationToolEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

		for (auto& Action : RegisterAssetTypeActions)
		{
			AssetTools.UnregisterAssetTypeActions(Action.ToSharedRef());
		}
	}

	FTerrainNodeFactory::Get().Clear();
	RegisterAssetTypeActions.Empty();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTerrainGenerationToolEditorModule, TerrainGenerationToolEditor)