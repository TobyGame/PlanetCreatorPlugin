#include "UI/Viewport/SUTK3DViewport.h"
#include "UI/Viewport/UTK3DViewportClient.h"

void SUTK3DViewport::Construct(const FArguments& InArgs)
{
	EditorApp = InArgs._EditorApp;
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
}

void SUTK3DViewport::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SEditorViewport::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}