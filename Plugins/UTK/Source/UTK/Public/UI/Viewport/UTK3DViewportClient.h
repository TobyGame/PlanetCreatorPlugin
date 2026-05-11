#pragma once

#include "CoreMinimal.h"
#include "EditorViewportClient.h"
#include "PreviewScene.h"
#include "Preview/UTKPreviewTerrainTypes.h"

class FUTKEditorApp;
class UStaticMeshComponent;
struct FUTKTerrain;
class AUTKTerrainPreviewActor;


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

	void FramePreview();
	void FocusOrigin();
	void ToggleGrid();
	bool IsGridEnabled() const;
	void ApplyDefaultView();

	void SetPreviewTerrain(
		const FUTKTerrain& Terrain,
		FName LayerName,
		const FUTKPreviewTerrainMapping& Mapping);

	void ClearPreviewTerrain();

private:
	void SetupPreviewScene();
	AUTKTerrainPreviewActor* EnsureTerrainPreviewActor();

	TWeakPtr<FUTKEditorApp> EditorApp;

	UStaticMeshComponent* FloorComponent = nullptr;
	TWeakObjectPtr<AUTKTerrainPreviewActor> TerrainPreviewActor;
};