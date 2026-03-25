#include "UI/Viewport/UTKViewportCommands.h"

#define LOCTEXT_NAMESPACE "UTKViewportCommands"

void FUTKViewportCommands::RegisterCommands()
{
	UI_COMMAND(
		FramePreview,
		"Frame Preview",
		"Frame the preview bounds.",
		EUserInterfaceActionType::Button,
		FInputChord(EKeys::F));

	UI_COMMAND(
		FocusOrigin,
		"Focus Origin",
		"Focus the viewport on the word origin.",
		EUserInterfaceActionType::Button,
		FInputChord());

	UI_COMMAND(
		ToggleGrid,
		"Toggle Grid",
		"Show or hide the preview grid",
		EUserInterfaceActionType::ToggleButton,
		FInputChord(EKeys::G));
}

#undef LOCTEXT_NAMESPACE