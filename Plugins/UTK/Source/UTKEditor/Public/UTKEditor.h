#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class IAssetTypeActions;

class FUTKEditor : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TArray<TSharedRef<IAssetTypeActions>> RegisterAssetTypeActions;
};