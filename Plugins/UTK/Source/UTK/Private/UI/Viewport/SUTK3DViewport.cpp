#include "UI/Viewport/SUTK3DViewport.h"
#include "UI/Viewport/SUTK3DViewportToolbar.h"
#include "UI/Viewport/UTK3DViewportClient.h"
#include "UI/Viewport/UTKViewportCommands.h"
#include "UI/Toolkit/UTKEditorApp.h"
#include "Core/UTKTerrainTypes.h"

void SUTK3DViewport::Construct(const FArguments& InArgs)
{
	EditorApp = InArgs._EditorApp;

	if (!FUTKViewportCommands::IsRegistered())
		FUTKViewportCommands::Register();

	SEditorViewport::Construct(SEditorViewport::FArguments());

	BindEditorPreviewDelegate();
}

SUTK3DViewport::~SUTK3DViewport()
{
	UnbindEditorPreviewDelegate();

	ViewportClient.Reset();
	ToolbarInfoProvider.Reset();
}

void SUTK3DViewport::BindEditorPreviewDelegate()
{
	TSharedPtr<FUTKEditorApp> PinnedEditor = EditorApp.Pin();
	if (!PinnedEditor.IsValid())
		return;

	if (!PreviewTerrainChangedHandle.IsValid())
	{
		PreviewTerrainChangedHandle =
			PinnedEditor->OnPreviewTerrainChanged().AddSP(
				SharedThis(this),
				&SUTK3DViewport::HandlePreviewTerrainChanged);
	}

	if (!PreviewTerrainClearedHandle.IsValid())
	{
		PreviewTerrainClearedHandle =
			PinnedEditor->OnPreviewTerrainCleared().AddSP(
				SharedThis(this),
				&SUTK3DViewport::HandlePreviewTerrainCleared);
	}
}

void SUTK3DViewport::UnbindEditorPreviewDelegate()
{
	TSharedPtr<FUTKEditorApp> PinnedEditor = EditorApp.Pin();

	if (PinnedEditor.IsValid())
	{
		if (PreviewTerrainChangedHandle.IsValid())
			PinnedEditor->OnPreviewTerrainChanged().Remove(PreviewTerrainChangedHandle);

		if (PreviewTerrainClearedHandle.IsValid())
			PinnedEditor->OnPreviewTerrainCleared().Remove(PreviewTerrainClearedHandle);
	}

	PreviewTerrainChangedHandle.Reset();
	PreviewTerrainClearedHandle.Reset();
}

void SUTK3DViewport::HandlePreviewTerrainChanged(const TSharedPtr<FUTKTerrain>& Terrain, FName LayerName, const FUTKPreviewTerrainMapping& Mapping)
{
	if (!ViewportClient.IsValid())
		return;

	if (!Terrain.IsValid() || !Terrain->IsValid() || LayerName.IsNone())
	{
		ViewportClient->ClearPreviewTerrain();
		return;
	}

	ViewportClient->SetPreviewTerrain(*Terrain, LayerName, Mapping);
}

void SUTK3DViewport::HandlePreviewTerrainCleared()
{
	if (ViewportClient.IsValid())
		ViewportClient->ClearPreviewTerrain();
}

TSharedRef<FEditorViewportClient> SUTK3DViewport::MakeEditorViewportClient()
{
	ViewportClient = MakeShared<FUTK3DViewportClient>(EditorApp, SharedThis(this));
	return ViewportClient.ToSharedRef();
}


TSharedPtr<SWidget> SUTK3DViewport::MakeViewportToolbar()
{
	if (!ToolbarInfoProvider.IsValid())
		ToolbarInfoProvider = MakeShared<FUTKViewportToolbarInfoProvider>(SharedThis(this));

	return SNew(SUTK3DViewportToolbar, ToolbarInfoProvider);
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