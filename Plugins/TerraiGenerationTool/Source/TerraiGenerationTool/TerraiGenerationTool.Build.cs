// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TerraiGenerationTool : ModuleRules
{
	public TerraiGenerationTool(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",

			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Projects",
			}
			);
	}
}
