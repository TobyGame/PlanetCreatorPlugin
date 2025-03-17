// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlanetaryToolsRuntimeModule.h"

#define LOCTEXT_NAMESPACE "FPlanetaryToolsModule"

void FPlanetaryToolsRuntimeModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("Planetary Tools Runtime Module Loaded"));
}

void FPlanetaryToolsRuntimeModule::ShutdownModule()
{
	UE_LOG(LogTemp, Log, TEXT("Planetary Tools Runtime Module Unloaded"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPlanetaryToolsRuntimeModule, PlanetaryToolsRuntime)