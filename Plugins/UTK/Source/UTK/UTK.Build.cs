// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UTK : ModuleRules
{
	public UTK(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", "CoreUObject", "Engine",
				"Slate", "SlateCore",
				"InputCore", "ApplicationCore"
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"UnrealEd",
				"EditorFramework",
				"Kismet",
				"GraphEditor",
				"PropertyEditor",
				"ToolMenus",
				"AssetTools",
				"Projects",
				"RenderCore"
			}
			);
	}
}
