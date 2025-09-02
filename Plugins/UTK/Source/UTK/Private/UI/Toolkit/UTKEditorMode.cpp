#include "UI/Toolkit/UTKEditorMode.h"
#include "UI/Toolkit/UTKEditorApp.h"
#include "UI/Tabs/UTKGraphTab.h"
#include "UI/Tabs/UTKViewportTab.h"
#include "UI/Tabs/UTKNodePropertiesTab.h"
#include "UI/Tabs/UTKLoggerTab.h"
#include "UI/Tabs/UTKGeneralPropertiesTab.h"

#define LOCTEXT_NAMESPACE "UTKEditorMode"

FUTKEditorMode::FUTKEditorMode(TSharedPtr<FUTKEditorApp> InEditor)
	: FApplicationMode("UTKEditorMode"), Editor(InEditor)
{
	TabsSet.RegisterFactory(MakeShared<FUTKGraphTab>(InEditor));
	TabsSet.RegisterFactory(MakeShared<FUTKViewportTab>(InEditor));
	TabsSet.RegisterFactory(MakeShared<FUTKNodePropertiesTab>(InEditor));
	TabsSet.RegisterFactory(MakeShared<FUTKGeneralPropertiesTab>(InEditor));
	TabsSet.RegisterFactory(MakeShared<FUTKLoggerTab>(InEditor));

	TabLayout = FTabManager::NewLayout("UTKEditorMode_Layout_v1")
		->AddArea(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Horizontal)
			->Split(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Vertical)
				->SetSizeCoefficient(0.75f)
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
				->SetSizeCoefficient(0.25f)
				->Split(
					FTabManager::NewStack()
					->AddTab("NodePropertiesTab", ETabState::OpenedTab)
					->AddTab("GeneralPropertiesTab", ETabState::OpenedTab)
				)
			)
		);
}

void FUTKEditorMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	TSharedPtr<FUTKEditorApp> Ed = Editor.Pin();

	if (!Ed.IsValid())
		return;

	Ed->PushTabFactories(TabsSet);
	FApplicationMode::RegisterTabFactories(InTabManager);
}

void FUTKEditorMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();
}

void FUTKEditorMode::PostActivateMode()
{
	FApplicationMode::PostActivateMode();
}

#undef LOCTEXT_NAMESPACE