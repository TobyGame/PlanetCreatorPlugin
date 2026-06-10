// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class IAssetTools;
class IAssetTypeActions;

class FUTKModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterShaderDirectory();
	void RegisterEditorIntegrations();
	void UnregisterEditorIntegrations();

private:
	bool bEditorIntegrationsRegistered = false;

	FDelegateHandle PostEngineInitHandle;

	TArray<TSharedPtr<IAssetTypeActions>> RegisteredAssetTypeActions;
};