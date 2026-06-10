// Copyright Epic Games, Inc. All Rights Reserved.

#include "UTK.h"

#include "AssetToolsModule.h"
#include "AssetTools/AssetTypeActions_UTKAsset.h"
#include "Core/UTKLogger.h"
#include "Framework/Commands/GenericCommands.h"
#include "Graph/Nodes/UTK_MathNodes.h"
#include "Graph/Nodes/UTKNodeFactory.h"
#include "IAssetTools.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/CoreDelegates.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "ShaderCore.h"

#define LOCTEXT_NAMESPACE "FUTKModule"

void FUTKModule::StartupModule()
{
	RegisterShaderDirectory();

	PostEngineInitHandle = FCoreDelegates::OnPostEngineInit.AddRaw(this, &FUTKModule::RegisterEditorIntegrations);
}

void FUTKModule::ShutdownModule()
{
	if (PostEngineInitHandle.IsValid())
	{
		FCoreDelegates::OnPostEngineInit.Remove(PostEngineInitHandle);
		PostEngineInitHandle.Reset();
	}

	UnregisterEditorIntegrations();

	FUTKNodeFactory::Get().Clear();
}

void FUTKModule::RegisterShaderDirectory()
{
	const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("UTK"));
	if (!Plugin.IsValid())
	{
		UE_LOG(LogUTKEditor, Warning, TEXT("[UTK] Failed to find UTK plugin while registering shader directory."));
		return;
	}

	const FString ShaderDirectory = FPaths::Combine(Plugin->GetBaseDir(), TEXT("Shaders"));

	AddShaderSourceDirectoryMapping(TEXT("/Plugin/UTK"), ShaderDirectory);

	UE_LOG(LogUTKEditor, Log, TEXT("[UTK] Registered shader directory: %s"), *ShaderDirectory);
}

void FUTKModule::RegisterEditorIntegrations()
{
	if (bEditorIntegrationsRegistered)
		return;

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	TSharedRef<IAssetTypeActions> Actions = MakeShareable(new FAssetTypeActions_UTKAsset());

	AssetTools.RegisterAssetTypeActions(Actions);
	RegisteredAssetTypeActions.Add(Actions);

	FGenericCommands::Register();

	RegisterMathNodes();

	bEditorIntegrationsRegistered = true;
}

void FUTKModule::UnregisterEditorIntegrations()
{
	if (!bEditorIntegrationsRegistered)
		return;

	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();

		for (const TSharedPtr<IAssetTypeActions>& Actions : RegisteredAssetTypeActions)
		{
			if (Actions.IsValid())
				AssetTools.UnregisterAssetTypeActions(Actions.ToSharedRef());
		}
	}

	RegisteredAssetTypeActions.Empty();
	bEditorIntegrationsRegistered = false;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUTKModule, UTK)