// Copyright Epic Games, Inc. All Rights Reserved.

#include "UTK.h"
#include "Modules/ModuleManager.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "AssetTools/AssetTypeActions_UTKAsset.h"
#include "Framework/Commands/GenericCommands.h"
#include "Graph/Nodes/UTK_MathNodes.h"
#include "Graph/Nodes/UTKNodeFactory.h"

#define LOCTEXT_NAMESPACE "FUTKModule"

void FUTKModule::StartupModule()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FAssetToolsModule::GetModule().Get();
		TSharedRef<IAssetTypeActions> Actions = MakeShareable(new FAssetTypeActions_UTKAsset());
		AssetTools.RegisterAssetTypeActions(Actions);
		RegisteredAssetTypeActions.Add(Actions);
	}

	FGenericCommands::Register();

	RegisterMathNodes();
}

void FUTKModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FAssetToolsModule::GetModule().Get();
		for (const TSharedPtr<IAssetTypeActions>& Actions : RegisteredAssetTypeActions)
		{
			if (Actions.IsValid())
			{
				AssetTools.UnregisterAssetTypeActions(Actions.ToSharedRef());
			}
		}
	}

	FUTKNodeFactory::Get().Clear();
	RegisteredAssetTypeActions.Empty();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUTKModule, UTK)