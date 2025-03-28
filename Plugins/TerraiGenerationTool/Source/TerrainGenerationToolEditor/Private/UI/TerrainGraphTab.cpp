#include "TerrainGraphTab.h"
#include "TerrainEditor.h"
#include "Graph/Core/TerrainGraph.h"
#include "Graph/Core/TerrainGraphSchema.h"

FTerrainGraphTab::FTerrainGraphTab(TSharedPtr<FTerrainEditor> InEditor)
	: FTerrainTabFactory(FName("GraphTab"), InEditor), TerrainEditor(InEditor)
{
	TabLabel = FText::FromString("Graph");
	bIsSingleton = true;
}

TSharedRef<SWidget> FTerrainGraphTab::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	TSharedPtr<FTerrainEditor> EditorPtr = TerrainEditor.Pin();
	if (!EditorPtr.IsValid())
	{
		return SNew(STextBlock).Text(FText::FromString("Invalid Editor context"));
	}

	UTerrainGraph* TerrainGraph = NewObject<UTerrainGraph>();
	TerrainGraph->Schema = UTerrainGraphSchema::StaticClass();

	FGraphAppearanceInfo Appearance;
	Appearance.CornerText = FText::FromString("Terrain Graph");

	SGraphEditor::FGraphEditorEvents Events;
	Events.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(EditorPtr.ToSharedRef(), &FTerrainEditor::OnGraphSelectionChanged);

	return SNew(SGraphEditor)
		.GraphToEdit(TerrainGraph)
		.Appearance(Appearance)
		.GraphEvents(Events)
		.ShowGraphStateOverlay(false);
}