#include "PlanetaryToolsEditor.h"

#define LOCTEXT_NAMESPACE "FPlanetaryToolsEditorModule"

void FPlanetaryToolsEditorModule::StartupModule()
{
    UE_LOG(LogTemp, Log, TEXT("Planetary Tools Editor Module Loaded"));
}

void FPlanetaryToolsEditorModule::ShutdownModule()
{
    UE_LOG(LogTemp, Log, TEXT("Planetary Tools Editor Module Unloaded"));
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FPlanetaryToolsEditorModule, PlanetaryToolsEditor)