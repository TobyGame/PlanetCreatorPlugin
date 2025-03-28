﻿#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class IAssetTypeActions;

class FTerrainGenerationToolEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    TArray<TSharedPtr<IAssetTypeActions>> RegisterAssetTypeActions;
};
