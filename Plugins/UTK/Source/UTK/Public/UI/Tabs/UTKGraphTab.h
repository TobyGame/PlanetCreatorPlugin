#pragma once

#include "UI/Tabs/UTKTabFactory.h"

class FUTKGraphTab : public FUTKTabFactory
{
public:
	FUTKGraphTab(TSharedPtr<FUTKEditorApp> InEditor);
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;

private:
	TWeakPtr<FUTKEditorApp> Editor;
};