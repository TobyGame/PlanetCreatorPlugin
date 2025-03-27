using System.IO;
using UnrealBuildTool;

public class TerrainGenerationToolEditor : ModuleRules
{
    public TerrainGenerationToolEditor(ReadOnlyTargetRules Target) : base(Target)
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
                "Projects",
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "UnrealEd",
                "GraphEditor",
                "EditorStyle",
                "PropertyEditor",
                "TerrainGenerationTool",
            }
        );
    }
}