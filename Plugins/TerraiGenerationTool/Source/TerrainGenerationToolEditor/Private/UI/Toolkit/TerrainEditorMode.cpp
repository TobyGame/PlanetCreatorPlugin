#include "Toolkit/TerrainEditorMode.h"
#include "Toolkit/TerrainEditor.h"
#include "TerrainTabIncludes.h"

#define LOCTEXT_NAMESPACE "TerrainEditorMode"

FTerrainEditorMode::FTerrainEditorMode(TSharedPtr<FTerrainEditor> InEditor)
	: FApplicationMode("TerrainEditorMode"), Editor(InEditor)
{
	TabsSet.RegisterFactory(MakeShared<FTerrainGraphTab>(InEditor));
	TabsSet.RegisterFactory(MakeShared<FTerrainViewportTab>(InEditor));
	TabsSet.RegisterFactory(MakeShared<FTerrainNodePropertiesTab>(InEditor));
	TabsSet.RegisterFactory(MakeShared<FTerrainGeneralPropertiesTab>(InEditor));
	TabsSet.RegisterFactory(MakeShared<FTerrainLoggerTab>(InEditor));

	TabLayout = FTabManager::NewLayout("TerrainEditorMode_Layout_v1")
		->AddArea(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Horizontal)
			->Split(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Vertical)
				->Split(
					FTabManager::NewStack()
					->AddTab("ViewportTab", ETabState::OpenedTab)
					->SetHideTabWell(true)
				)
				->Split(
					FTabManager::NewStack()
					->AddTab("GraphTab", ETabState::OpenedTab)
				)
				->Split(
					FTabManager::NewStack()
					->AddTab("LoggerTab", ETabState::ClosedTab)
				)
			)
			->Split(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Vertical)
				->Split(
					FTabManager::NewStack()
					->AddTab("NodePropertiesTab", ETabState::OpenedTab)
					->AddTab("GeneralPropertiesTab", ETabState::OpenedTab)
				)
			)
		);
}

void FTerrainEditorMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	TSharedPtr<FTerrainEditor> Ed = Editor.Pin();

	if (!Ed.IsValid())
		return;

	Ed->PushTabFactories(TabsSet);
	FApplicationMode::RegisterTabFactories(InTabManager);
}

void FTerrainEditorMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();
}

void FTerrainEditorMode::PostActivateMode()
{
	FApplicationMode::PostActivateMode();
}

#undef LOCTEXT_NAMESPACE