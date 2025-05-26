#include "TerrainGraphTab.h"
#include "Toolkit/TerrainEditor.h"
#include "Graph/TerrainGraph.h"

FTerrainGraphTab::FTerrainGraphTab(TSharedPtr<FTerrainEditor> InEditor)
	: FTerrainTabFactory(FName("GraphTab"), InEditor), Editor(InEditor)
{
	TabLabel = FText::FromString("Graph");
	bIsSingleton = true;
}

TSharedRef<SWidget> FTerrainGraphTab::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	TSharedPtr<FTerrainEditor> EditorPtr = Editor.Pin();
	if (!EditorPtr.IsValid())
	{
		return SNew(STextBlock).Text(FText::FromString("Invalid Editor context"));
	}

	UTerrainGraph* TerrainGraph = EditorPtr->GetGraph();
	if (!TerrainGraph)
	{
		return SNew(STextBlock).Text(FText::FromString("No graph loaded"));
	}

	FGraphAppearanceInfo Appearance;
	Appearance.CornerText = FText::FromString("Terrain Graph");

	TSharedPtr<SGraphEditor> GraphEditor =
		SNew(SGraphEditor)
		.IsEditable(true)
		.GraphToEdit(EditorPtr->GetGraph())
		.AdditionalCommands(EditorPtr->GetToolkitCommands());

	EditorPtr->SetWorkingGraphUI(GraphEditor);

	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.HAlign(HAlign_Fill)
		[
			GraphEditor.ToSharedRef()
		];
}