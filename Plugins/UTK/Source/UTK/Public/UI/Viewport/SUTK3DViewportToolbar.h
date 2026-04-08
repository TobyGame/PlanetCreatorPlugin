#pragma once

#include "CoreMinimal.h"
#include "SCommonEditorViewportToolbarBase.h"

class SUTK3DViewport;

class FUTKViewportToolbarInfoProvider : public ICommonEditorViewportToolbarInfoProvider
{
public:
	explicit FUTKViewportToolbarInfoProvider(TWeakPtr<SUTK3DViewport> InViewportWidget)
		: ViewportWidget(InViewportWidget)
	{}

	virtual ~FUTKViewportToolbarInfoProvider() = default;

	virtual TSharedRef<SEditorViewport> GetViewportWidget() override;
	virtual TSharedPtr<FExtender> GetExtenders() const override;
	virtual void OnFloatingButtonClicked() override;

private:
	TWeakPtr<SUTK3DViewport> ViewportWidget;
};

class SUTK3DViewportToolbar : public SCommonEditorViewportToolbarBase
{
public:
	SLATE_BEGIN_ARGS(SUTK3DViewportToolbar) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TSharedPtr<FUTKViewportToolbarInfoProvider> InInfoProvider);

	virtual TSharedRef<SWidget> GenerateShowMenu() const override;

private:
	TSharedPtr<FUTKViewportToolbarInfoProvider> InfoProvider;
};