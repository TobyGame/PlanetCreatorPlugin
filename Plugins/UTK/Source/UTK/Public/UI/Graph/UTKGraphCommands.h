#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Styling/AppStyle.h"
#include "InputCoreTypes.h"

class FUTKGraphCommands : public TCommands<FUTKGraphCommands>
{
public:
	FUTKGraphCommands()
		: TCommands<FUTKGraphCommands>(
			TEXT("UTKGraph"),
			NSLOCTEXT("Contexts", "UTKGraph", "UTK Graph"),
			NAME_None,
			FAppStyle::GetAppStyleSetName())
	{}

	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> LockPreview;
};