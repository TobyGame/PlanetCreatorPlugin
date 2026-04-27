#include "UI/Graph/UTKGraphCommands.h"

#define LOCTEXT_NAMESPACE "UTKGraphCommands"

void FUTKGraphCommands::RegisterCommands()
{
	UI_COMMAND(
		LockPreview,
		"Lock Preview",
		"Lock or unlock the preview on the selected node.",
		EUserInterfaceActionType::Button,
		FInputChord(EKeys::F));
}

#undef LOCTEXT_NAMESPACE