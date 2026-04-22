#include "UI/Viewport/UTK3DViewportClient.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"


FUTK3DViewportClient::FUTK3DViewportClient(TWeakPtr<FUTKEditorApp> InEditorApp, const TSharedRef<SEditorViewport>& InViewportWidget)
	: FUTKPreviewSceneHolder()
	  , FEditorViewportClient(nullptr, &UTKPreviewScene, InViewportWidget)
	  , EditorApp(InEditorApp)
{
	EngineShowFlags.SetGrid(true);
	DrawHelper.bDrawGrid = true;
	DrawHelper.bDrawPivot = false;
	EngineShowFlags.SetSelectionOutline(false);
	EngineShowFlags.SetSelection(false);
	EngineShowFlags.SetModeWidgets(false);
	EngineShowFlags.SetPivot(false);

	EngineShowFlags.SetTemporalAA(false);
	EngineShowFlags.SetAntiAliasing(false);

	FEditorViewportClient::SetViewMode(VMI_Lit);
	SetRealtime(false);
	SetCameraSpeedSetting(2);

	SetupPreviewScene();
	ApplyDefaultView();
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


void FUTK3DViewportClient::FramePreview()
{
	ApplyDefaultView();
}

void FUTK3DViewportClient::FocusOrigin()
{
	ApplyDefaultView();
}

void FUTK3DViewportClient::ToggleGrid()
{
	const bool bNewState = !DrawHelper.bDrawGrid;

	DrawHelper.bDrawGrid = bNewState;
	EngineShowFlags.SetGrid(bNewState);

	Invalidate();
}

bool FUTK3DViewportClient::IsGridEnabled() const
{
	return DrawHelper.bDrawGrid;
}

void FUTK3DViewportClient::ApplyDefaultView()
{
	SetViewLocation(FVector(-115.0f, -115.0f, 75.f));
	SetViewRotation(FRotator(-28.0f, 45.0f, 0.0f));

	SetLookAtLocation(FVector::ZeroVector);

	Invalidate();
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
			PlaneComp->SetRelativeScale3D(FVector(100.0f, 100.0f, 1.0f));

			UTKPreviewScene.AddComponent(PlaneComp, FTransform::Identity);
			FloorComponent = PlaneComp;
		}
	}
}

FBox FUTK3DViewportClient::GetDefaultPreviewBounds() const
{
	return FBox(
		FVector(-250.0f, -250.0f, -25.0f),
		FVector(250.0f, 250.0f, 25.0f));
}