#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Styling/AppStyle.h"
#include "InputCoreTypes.h"

class FUTKViewportCommands : public TCommands<FUTKViewportCommands>
{
public:
	FUTKViewportCommands()
		: TCommands<FUTKViewportCommands>(
			TEXT("UTKViewport"),
			NSLOCTEXT("Contexts", "UTKViewport", "UTK Viewport"),
			NAME_None,
			FAppStyle::GetAppStyleSetName())
	{}

	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> FramePreview;
	TSharedPtr<FUICommandInfo> FocusOrigin;
	TSharedPtr<FUICommandInfo> ToggleGrid;
};