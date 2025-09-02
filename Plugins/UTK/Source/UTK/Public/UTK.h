// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "tiff.h"
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
	TArray<TSharedPtr<IAssetTypeActions>> RegisteredAssetTypeActions;
};