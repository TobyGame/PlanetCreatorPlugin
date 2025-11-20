#pragma once

#include "UI/Tabs/UTKTabFactory.h"

class FUTKEditorApp;
class IDetailsView;
class UUTKNode;

class FUTKNodePropertiesTab : public FUTKTabFactory
{
public:
	FUTKNodePropertiesTab(TSharedPtr<FUTKEditorApp> InEditor);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;

private:
	TWeakPtr<FUTKEditorApp> Editor;

	mutable TSharedPtr<IDetailsView> DetailsView;

	void HandleSelectedNodeChanged(UUTKNode* NewNode);
};