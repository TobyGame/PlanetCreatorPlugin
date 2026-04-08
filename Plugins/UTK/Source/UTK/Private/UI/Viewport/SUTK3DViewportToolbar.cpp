#include "UI/Viewport/SUTK3DViewportToolbar.h"
#include "UI/Viewport/SUTK3DViewport.h"
#include "UI/Viewport/UTKViewportCommands.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

TSharedRef<SEditorViewport> FUTKViewportToolbarInfoProvider::GetViewportWidget()
{
	TSharedPtr<SUTK3DViewport> PinnedViewport = ViewportWidget.Pin();
	check(PinnedViewport.IsValid());
	return StaticCastSharedRef<SEditorViewport>(PinnedViewport.ToSharedRef());
}

TSharedPtr<FExtender> FUTKViewportToolbarInfoProvider::GetExtenders() const
{
	return MakeShared<FExtender>(); // Safe empty extender
}

void FUTKViewportToolbarInfoProvider::OnFloatingButtonClicked()
{
	// Not used for now
}

void SUTK3DViewportToolbar::Construct(const FArguments& InArgs, TSharedPtr<FUTKViewportToolbarInfoProvider> InInfoProvider)
{
	InfoProvider = InInfoProvider;

	SCommonEditorViewportToolbarBase::Construct(
		SCommonEditorViewportToolbarBase::FArguments(),
		InfoProvider
	);
}

TSharedRef<SWidget> SUTK3DViewportToolbar::GenerateShowMenu() const
{
	TSharedRef<SUTK3DViewport> ViewportRef = StaticCastSharedRef<SUTK3DViewport>(InfoProvider->GetViewportWidget());

	FMenuBuilder MenuBuilder(
		true,
		ViewportRef->GetCommandList()
	);

	const FUTKViewportCommands& Commands = FUTKViewportCommands::Get();

	MenuBuilder.BeginSection("UTKViewport", FText::FromString(TEXT("Viewport")));
	{
		MenuBuilder.AddMenuEntry(Commands.FramePreview);
		MenuBuilder.AddMenuEntry(Commands.FocusOrigin);
		MenuBuilder.AddMenuEntry(Commands.ToggleGrid);
	}
	MenuBuilder.EndSection();

	return MenuBuilder.MakeWidget();
}