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
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "UnrealEd",
                "GraphEditor",
                "EditorStyle",
                "Kismet",
                "KismetCompiler",
                "BlueprintGraph",
                "WorkspaceMenuStructure",
                "ApplicationCore",
            }
            );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {

                "TerrainGenerationTool",
            }
        );

        DynamicallyLoadedModuleNames.Add("AssetTools");
    }
}