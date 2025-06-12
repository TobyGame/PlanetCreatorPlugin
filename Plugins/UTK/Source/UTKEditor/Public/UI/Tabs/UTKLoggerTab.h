#pragma once

#include "Tabs/UTKTabFactory.h"

class FUTKLoggerTab : public FUTKTabFactory
{
public:
	FUTKLoggerTab(TSharedPtr<FUTKEditorApp> InEditor);
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;

private:
	TWeakPtr<FUTKEditorApp> Editor;
};