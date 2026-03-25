#include "UI/Viewport/SUTK3DViewport.h"
#include "UI/Viewport/UTK3DViewportClient.h"
#include "UI/Viewport/UTKViewportCommands.h"

void SUTK3DViewport::Construct(const FArguments& InArgs)
{
	EditorApp = InArgs._EditorApp;

	if (!FUTKViewportCommands::IsRegistered())
		FUTKViewportCommands::Register();

	SEditorViewport::Construct(SEditorViewport::FArguments());
}

SUTK3DViewport::~SUTK3DViewport()
{
	ViewportClient.Reset();
}

TSharedRef<FEditorViewportClient> SUTK3DViewport::MakeEditorViewportClient()
{
	ViewportClient = MakeShared<FUTK3DViewportClient>(EditorApp, SharedThis(this));
	return ViewportClient.ToSharedRef();
}

void SUTK3DViewport::BindCommands()
{
	SEditorViewport::BindCommands();

	const FUTKViewportCommands& Commands = FUTKViewportCommands::Get();

	CommandList->MapAction(
		Commands.FramePreview,
		FExecuteAction::CreateSP(this, &SUTK3DViewport::HandleFramePreview)
	);

	CommandList->MapAction(
		Commands.FocusOrigin,
		FExecuteAction::CreateSP(this, &SUTK3DViewport::HandleFocusOrigin)
	);

	CommandList->MapAction(
		Commands.ToggleGrid,
		FExecuteAction::CreateSP(this, &SUTK3DViewport::HandleToggleGrid),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &SUTK3DViewport::IsGridEnabled)
	);

}

void SUTK3DViewport::HandleFramePreview()
{
	if (ViewportClient.IsValid())
	{
		ViewportClient->FramePreview();
	}
}

void SUTK3DViewport::HandleFocusOrigin()
{
	if (ViewportClient.IsValid())
	{
		ViewportClient->FocusOrigin();
	}
}

void SUTK3DViewport::HandleToggleGrid()
{
	if (ViewportClient.IsValid())
	{
		ViewportClient->ToggleGrid();
	}
}

bool SUTK3DViewport::IsGridEnabled() const
{
	return ViewportClient.IsValid() ? ViewportClient->IsGridEnabled() : false;
}

void SUTK3DViewport::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SEditorViewport::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}