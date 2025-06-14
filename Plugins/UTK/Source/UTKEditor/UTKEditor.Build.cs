﻿using System.IO;
using UnrealBuildTool;

public class UTKEditor : ModuleRules
{
    public UTKEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePaths.AddRange(
            new string[]
            {
                Path.Combine(ModuleDirectory, "Public/UI"),
                Path.Combine(ModuleDirectory, "Public/UI/Tabs"),
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Projects",
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "UnrealEd",
                "GraphEditor",
                "EditorStyle",
                "PropertyEditor",
                "UTK",
                "StructUtils",
                "ToolMenus",
            }
        );
    }
}