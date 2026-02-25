#pragma once

#include "CoreMinimal.h"
#include "EditorViewportClient.h"
#include "PreviewScene.h"

class FUTKEditorApp;
class UStaticMeshComponent;


class FUTKPreviewSceneHolder
{
public:
	FUTKPreviewSceneHolder()
		: UTKPreviewScene(FPreviewScene::ConstructionValues())
	{}

protected:
	FPreviewScene UTKPreviewScene;
};


class FUTK3DViewportClient : public FUTKPreviewSceneHolder, public FEditorViewportClient
{
public:
	FUTK3DViewportClient(TWeakPtr<FUTKEditorApp> InEditorApp, const TSharedRef<SEditorViewport>& InViewportWidget);

	virtual ~FUTK3DViewportClient();

	// FEditorViewportClient
	virtual FLinearColor GetBackgroundColor() const override;

private:
	void SetupPreviewScene();

	TWeakPtr<FUTKEditorApp> EditorApp;
	UStaticMeshComponent* FloorComponent = nullptr;
};