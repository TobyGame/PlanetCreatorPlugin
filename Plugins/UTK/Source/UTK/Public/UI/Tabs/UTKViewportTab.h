#pragma once

#include "UI/Tabs/UTKTabFactory.h"

class FUTKViewportTab : public FUTKTabFactory
{
public:
	FUTKViewportTab(TSharedPtr<FUTKEditorApp> InEditor);
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;

private:
	TWeakPtr<FUTKEditorApp> Editor;

	mutable FSlateBrush PreviewBrush;

	const FSlateBrush* GetPreviewBrush() const;

	void RefreshOutputPinOptions() const;

	mutable TArray<TSharedPtr<FName>> OutputPinOptions;

	mutable TSharedPtr<FName> CurrentOutputSelection;

	mutable TSharedPtr<SComboBox<TSharedPtr<FName>>> OutputComboBox;

	FText GetCurrentOutputSelectionText() const;
};