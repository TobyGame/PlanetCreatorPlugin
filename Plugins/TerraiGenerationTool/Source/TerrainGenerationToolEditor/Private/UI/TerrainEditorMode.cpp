#include "TerrainEditorMode.h"
#include "TerrainEditor.h"
#include "TerrainGraphTab.h"
#include "TerrainViewportTab.h"
#include "TerrainNodePropertiesTab.h"
#include "TerrainGeneralPropertiesTab.h"
#include "TerrainLoggerTab.h"

#define LOCTEXT_NAMESPACE "TerrainEditorMode"

FTerrainEditorMode::FTerrainEditorMode(TSharedPtr<FTerrainEditor> InEditor)
	: FApplicationMode("TerrainEditorMode"), TerrainEditorWeak(InEditor)
{
	TabsSet.RegisterFactory(MakeShareable(new FTerrainGraphTab(InEditor)));
	TabsSet.RegisterFactory(MakeShareable(new FTerrainViewportTab(InEditor)));
	TabsSet.RegisterFactory(MakeShareable(new FTerrainNodePropertiesTab(InEditor)));
	TabsSet.RegisterFactory(MakeShareable(new FTerrainGeneralPropertiesTab(InEditor)));
	TabsSet.RegisterFactory(MakeShareable(new FTerrainLoggerTab(InEditor)));

	TabLayout = FTabManager::NewLayout("TerrainEditorMode_Layout")
		->AddArea(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Horizontal)
			->Split(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Vertical)
				->Split(
					FTabManager::NewStack()
					->AddTab("ViewportTab", ETabState::OpenedTab)
					->SetSizeCoefficient(0.6f)
					->SetHideTabWell(true)
				)
				->Split(
					FTabManager::NewStack()
					->AddTab("GraphTab", ETabState::OpenedTab)
					->SetSizeCoefficient(0.4f)
					->SetHideTabWell(false)
				)
			)
			->Split(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.25f)
				->SetHideTabWell(false)
				->AddTab("NodePropertiesTab", ETabState::OpenedTab)
				->AddTab("GeneralPropertiesTab", ETabState::OpenedTab)
			)
		);
}

void FTerrainEditorMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	TSharedPtr<FTerrainEditor> Editor = TerrainEditorWeak.Pin();

	if (!Editor.IsValid())
		return;

	Editor->PushTabFactories(TabsSet);
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