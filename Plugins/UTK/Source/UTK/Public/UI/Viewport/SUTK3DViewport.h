#pragma once

#include "CoreMinimal.h"
#include "SEditorViewport.h"
#include "SCommonEditorViewportToolbarBase.h"

class FUTK3DViewportClient;
class FUTKEditorApp;

class SUTK3DViewport : public SEditorViewport, public ICommonEditorViewportToolbarInfoProvider
{
public:
	SLATE_BEGIN_ARGS(SUTK3DViewport) {}
		SLATE_ARGUMENT(TWeakPtr<FUTKEditorApp>, EditorApp)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual ~SUTK3DViewport();

	// SEditorViewport
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	virtual void BindCommands() override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	// ICommonEditorViewportToolbarInfoProvider
	virtual TSharedRef<SEditorViewport> GetViewportWidget() override { return SharedThis(this); }
	virtual TSharedPtr<FExtender> GetExtenders() const override { return nullptr; }
	virtual void OnFloatingButtonClicked() override {}

	TSharedPtr<FUTK3DViewportClient> GetUtkViewportClient() const { return ViewportClient; }

private:
	TWeakPtr<FUTKEditorApp> EditorApp;
	TSharedPtr<FUTK3DViewportClient> ViewportClient;
};