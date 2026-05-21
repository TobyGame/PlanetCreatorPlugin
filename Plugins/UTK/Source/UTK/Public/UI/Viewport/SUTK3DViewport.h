#pragma once

#include "CoreMinimal.h"
#include "SEditorViewport.h"

class FUTK3DViewportClient;
class FUTKEditorApp;
class FUTKViewportToolbarInfoProvider;

struct FUTKTerrain;
struct FUTKPreviewTerrainMapping;

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
	virtual FReply OnPreviewKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	TSharedPtr<FUTK3DViewportClient> GetUtkViewportClient() const { return ViewportClient; }

private:
	void HandleFramePreview();
	void HandleFocusOrigin();
	void HandleToggleGrid();
	bool IsGridEnabled() const;

	void BindEditorPreviewDelegate();
	void UnbindEditorPreviewDelegate();

	void RefreshFlatPreviewFromEditorSettings();
	void HandlePreviewTerrainChanged(const TSharedPtr<FUTKTerrain>& Terrain, FName LayerName, const FUTKPreviewTerrainMapping& Mapping);
	void HandlePreviewTerrainCleared(const FUTKPreviewTerrainMapping& Mapping);

	TWeakPtr<FUTKEditorApp> EditorApp;
	TSharedPtr<FUTK3DViewportClient> ViewportClient;
	TSharedPtr<FUTKViewportToolbarInfoProvider> ToolbarInfoProvider;

	FDelegateHandle PreviewTerrainChangedHandle;
	FDelegateHandle PreviewTerrainClearedHandle;
};