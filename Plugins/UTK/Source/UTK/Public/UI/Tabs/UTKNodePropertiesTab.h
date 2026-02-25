#pragma once

#include "UI/Tabs/UTKTabFactory.h"

class FUTKEditorApp;
class IDetailsView;
class UUTKNode;

class FUTKNodePropertiesTab : public FUTKTabFactory
{
public:
	FUTKNodePropertiesTab(TSharedPtr<FUTKEditorApp> InEditor);
	virtual ~FUTKNodePropertiesTab();

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;

private:
	TWeakPtr<FUTKEditorApp> Editor;

	mutable TSharedPtr<IDetailsView> DetailsView;

	mutable FDelegateHandle SelectedNodeChangedHandle;

	void HandleSelectedNodeChanged(UUTKNode* NewNode);
};