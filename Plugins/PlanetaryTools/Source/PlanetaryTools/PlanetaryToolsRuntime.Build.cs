// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PlanetaryToolsRuntime : ModuleRules
{
	public PlanetaryToolsRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"ProceduralMeshComponent",
				"MeshDescription",
				"StaticMeshDescription",
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
