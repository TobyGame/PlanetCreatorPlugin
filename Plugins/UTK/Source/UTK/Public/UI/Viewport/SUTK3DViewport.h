#pragma once

#include "CoreMinimal.h"
#include "SEditorViewport.h"

class FUTK3DViewportClient;
class FUTKEditorApp;
class FUTKViewportToolbarInfoProvider;

class SUTK3DViewport : public SEditorViewport
{
public:
	SLATE_BEGIN_ARGS(SUTK3DViewport) {}
		SLATE_ARGUMENT(TWeakPtr<FUTKEditorApp>, EditorApp)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual ~SUTK3DViewport();

	// SEditorViewport
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	virtual TSharedPtr<SWidget> MakeViewportToolbar() override;
	virtual void BindCommands() override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	TSharedPtr<FUTK3DViewportClient> GetUtkViewportClient() const { return ViewportClient; }

private:
	void HandleFramePreview();
	void HandleFocusOrigin();
	void HandleToggleGrid();
	bool IsGridEnabled() const;

	TWeakPtr<FUTKEditorApp> EditorApp;
	TSharedPtr<FUTK3DViewportClient> ViewportClient;
	TSharedPtr<FUTKViewportToolbarInfoProvider> ToolbarInfoProvider;
};