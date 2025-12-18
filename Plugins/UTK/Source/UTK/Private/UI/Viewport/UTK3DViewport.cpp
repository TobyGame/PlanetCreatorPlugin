#include "UI/Viewport/UTK3DViewport.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"


FUTK3DViewportClient::FUTK3DViewportClient(TWeakPtr<FUTKEditorApp> InEditorApp, const TSharedRef<SEditorViewport>& InViewportWidget)
	: FEditorViewportClient(nullptr, &PreviewScene, InViewportWidget)
	  , EditorApp(InEditorApp)
{
	EngineShowFlags.SetGrid(true);
	DrawHelper.bDrawGrid = true;
	DrawHelper.bDrawPivot = true;
	EngineShowFlags.SetSelectionOutline(false);
	EngineShowFlags.SetSelection(false);
	EngineShowFlags.SetModeWidgets(false);
	EngineShowFlags.SetPivot(false);

	EngineShowFlags.SetTemporalAA(false);
	EngineShowFlags.SetAntiAliasing(false);

	FEditorViewportClient::SetViewMode(VMI_Lit);
	SetRealtime(false);

	SetViewLocation(FVector(-2000.0f, -2000.0f, 1200.0f));
	SetViewRotation(FRotator(-25.0f, 45.0f, 0.0f));
	FEditorViewportClient::SetCameraSpeedSetting(2);

	SetupPreviewScene();
}

FLinearColor FUTK3DViewportClient::GetBackgroundColor() const
{
	return FLinearColor(0.03f, 0.03f, 0.03f, 1.0f);
}

void FUTK3DViewportClient::SetupPreviewScene()
{
	if (PreviewScene.DirectionalLight)
	{
		PreviewScene.DirectionalLight->Intensity = 3.0f;
		PreviewScene.DirectionalLight->SetRelativeRotation(FRotator(-45.0f, 45.0f, 0.0f));
	}

	if (PreviewScene.SkyLight)
	{
		PreviewScene.SkyLight->Intensity = 0.5f;
		PreviewScene.SkyLight->bRealTimeCapture = false;
	}
}