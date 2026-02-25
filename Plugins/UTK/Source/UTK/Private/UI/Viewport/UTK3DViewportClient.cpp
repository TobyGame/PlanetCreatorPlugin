#include "UI/Viewport/UTK3DViewportClient.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"


FUTK3DViewportClient::FUTK3DViewportClient(TWeakPtr<FUTKEditorApp> InEditorApp, const TSharedRef<SEditorViewport>& InViewportWidget)
	: FUTKPreviewSceneHolder()
	  , FEditorViewportClient(nullptr, &UTKPreviewScene, InViewportWidget)
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

FUTK3DViewportClient::~FUTK3DViewportClient()
{
	Viewport = nullptr;

	if (FloorComponent)
	{
		UTKPreviewScene.RemoveComponent(FloorComponent);
		FloorComponent = nullptr;
	}
}

FLinearColor FUTK3DViewportClient::GetBackgroundColor() const
{
	return FLinearColor(0.03f, 0.03f, 0.03f, 1.0f);
}

void FUTK3DViewportClient::SetupPreviewScene()
{
	if (UTKPreviewScene.DirectionalLight)
	{
		UTKPreviewScene.DirectionalLight->Intensity = 3.0f;
		UTKPreviewScene.DirectionalLight->SetRelativeRotation(FRotator(-45.0f, 45.0f, 0.0f));
	}

	if (UTKPreviewScene.SkyLight)
	{
		UTKPreviewScene.SkyLight->Intensity = 0.5f;
		UTKPreviewScene.SkyLight->bRealTimeCapture = false;
	}

	if (!FloorComponent)
	{
		UStaticMesh* PlaneMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane"));
		if (PlaneMesh)
		{
			UStaticMeshComponent* PlaneComp = NewObject<UStaticMeshComponent>(
				GetTransientPackage(),
				NAME_None,
				RF_Transient
			);
			PlaneComp->SetStaticMesh(PlaneMesh);
			PlaneComp->SetMobility(EComponentMobility::Movable);
			PlaneComp->SetCastShadow(false);
			PlaneComp->SetRelativeScale3D(FVector(200.0f, 200.0f, 1.0f));

			UTKPreviewScene.AddComponent(PlaneComp, FTransform::Identity);
			FloorComponent = PlaneComp;
		}
	}
}