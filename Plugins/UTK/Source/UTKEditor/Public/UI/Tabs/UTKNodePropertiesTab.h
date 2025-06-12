﻿#pragma once

#include "Tabs/UTKTabFactory.h"


class FUTKNodePropertiesTab : public FUTKTabFactory
{
public:
	FUTKNodePropertiesTab(TSharedPtr<FUTKEditorApp> InEditor);
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;

private:
	TWeakPtr<FUTKEditorApp> Editor;
};