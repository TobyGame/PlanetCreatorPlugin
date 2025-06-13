#include "UTKEditor.h"
#include "AssetToolsModule.h"
#include "AssetTools/UTKAssetFactory.h"
#include "AssetTools/AssetTypeActions_UTKAsset.h"
#include "Core/UTKLoggerEditor.h"
#include "Framework/Commands/GenericCommands.h"
#include "Graph/Nodes/UTK_MathNodes.h"
#include "Graph/Nodes/UTKNodeFactory.h"

#define LOCTEXT_NAMESPACE "FUTKEditor"

void FUTKEditor::StartupModule()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	TSharedRef<FAssetTypeActions_UTKAsset> UTKAssetTypeActions = MakeShared<FAssetTypeActions_UTKAsset>();
	AssetTools.RegisterAssetTypeActions(UTKAssetTypeActions);
	RegisterAssetTypeActions.Add(UTKAssetTypeActions);
	UE_LOG(LogUTKEditor, Log, TEXT("Register UTK asset type actions."));

	FGenericCommands::Register();

	RegisterMathNodes();
}

void FUTKEditor::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

		for (auto& Action : RegisterAssetTypeActions)
		{
			AssetTools.UnregisterAssetTypeActions(Action);
			UE_LOG(LogUTKEditor, Log, TEXT("Unregister UTK asset type actions."));
		}
	}

	FUTKNodeFactory::Get().Clear();
	RegisterAssetTypeActions.Empty();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUTKEditor, UTKEditor)