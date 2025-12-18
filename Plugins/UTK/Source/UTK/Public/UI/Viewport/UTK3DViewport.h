#pragma once

#include "CoreMinimal.h"
#include "EditorViewportClient.h"
#include "PreviewScene.h"

class FUTKEditorApp;

class FUTK3DViewportClient : public FEditorViewportClient
{
public:
	FUTK3DViewportClient(TWeakPtr<FUTKEditorApp> InEditorApp, const TSharedRef<SEditorViewport>& InViewportWidget);

	// FEditorViewportClient
	virtual FLinearColor GetBackgroundColor() const override;

private:
	void SetupPreviewScene();

	TWeakPtr<FUTKEditorApp> EditorApp;
	FPreviewScene PreviewScene;
};