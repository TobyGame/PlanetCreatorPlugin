#pragma once

#include "UI/Tabs/UTKTabFactory.h"

class FUTKViewportTab : public FUTKTabFactory
{
public:
	FUTKViewportTab(TSharedPtr<FUTKEditorApp> InEditor);
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;

private:
	TWeakPtr<FUTKEditorApp> Editor;
};