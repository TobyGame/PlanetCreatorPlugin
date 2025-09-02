#include "UI/Tabs/UTKGraphTab.h"
#include "UI/Toolkit/UTKEditorApp.h"
#include "Graph/UTKGraph.h"

FUTKGraphTab::FUTKGraphTab(TSharedPtr<FUTKEditorApp> InEditor)
	: FUTKTabFactory(FName("GraphTab"), InEditor), Editor(InEditor)
{
	TabLabel = FText::FromString("Graph");
	bIsSingleton = true;
}

TSharedRef<SWidget> FUTKGraphTab::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	TSharedPtr<FUTKEditorApp> EditorPtr = Editor.Pin();
	if (!EditorPtr.IsValid())
	{
		return SNew(STextBlock).Text(FText::FromString("Invalid Editor context"));
	}

	UUTKGraph* UTKGraph = EditorPtr->GetGraph();
	if (!UTKGraph)
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