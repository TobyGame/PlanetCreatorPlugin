#pragma once

#include "UI/Tabs/UTKTabFactory.h"

class FUTKGeneralPropertiesTab : public FUTKTabFactory
{
public:
	FUTKGeneralPropertiesTab(TSharedPtr<FUTKEditorApp> InEditor);
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;

private:
	TWeakPtr<FUTKEditorApp> Editor;
};