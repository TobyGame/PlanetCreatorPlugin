#include "Preview/UTKTerrainPreviewComponent.h"

#include "Components/DynamicMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Core/UTKTerrainTypes.h"
#include "DynamicMesh/DynamicMesh3.h"
#include "DynamicMesh/MeshNormals.h"
#include "GameFramework/Actor.h"
#include "Materials/Material.h"


namespace
{
	constexpr int32 UTKPreviewMinDynamicMeshResolution = 2;
	constexpr int32 UTKPreviewMaxDynamicMeshResolution = 4096;

	int32 ComputeDynamicMeshResolution(const FUTKBuffer2D& Buffer, const FUTKPreviewTerrainMapping& Mapping)
	{
		if (!Buffer.IsValid())
			return 0;

		const int32 RequestedResolution = Mapping.Resolution > 0
			? Mapping.Resolution
			: FMath::Min(Buffer.Width, Buffer.Height);

		const int32 SourceLimitedResolution = FMath::Min3(
			RequestedResolution,
			Buffer.Width,
			Buffer.Height);

		return FMath::Clamp(
			SourceLimitedResolution,
			UTKPreviewMinDynamicMeshResolution,
			UTKPreviewMaxDynamicMeshResolution);
	}

	int32 ComputeDynamicMeshResolution(const FUTKPreviewTerrainMapping& Mapping)
	{
		const int32 RequestedResolution = Mapping.Resolution > 0
			? Mapping.Resolution
			: 512;

		return FMath::Clamp(
			RequestedResolution,
			UTKPreviewMinDynamicMeshResolution,
			UTKPreviewMaxDynamicMeshResolution);
	}

	float SampleLayerNearest(const FUTKBuffer2D& Buffer, float U, float V)
	{
		if (!Buffer.IsValid())
			return 0.0f;

		const int32 X = FMath::Clamp(
			FMath::RoundToInt(U * static_cast<float>(Buffer.Width - 1)),
			0,
			Buffer.Width - 1);

		const int32 Y = FMath::Clamp(
			FMath::RoundToInt(V * static_cast<float>(Buffer.Height - 1)),
			0,
			Buffer.Height - 1);

		return Buffer.Get(X, Y);
	}

	template <typename THeightSampler>
	bool BuildDynamicMeshGrid(int32 RenderResolution, const FUTKPreviewTerrainMapping& Mapping, THeightSampler&& HeightSampler, UE::Geometry::FDynamicMesh3& OutMesh)
	{
		if (RenderResolution < UTKPreviewMinDynamicMeshResolution)
			return false;

		UE::Geometry::FDynamicMesh3 Mesh(
			true,
			false,
			false,
			true);

		TArray<int32> VertexIds;
		VertexIds.SetNum(RenderResolution * RenderResolution);

		const auto VertexIndex = [RenderResolution](int32 X, int32 Y){
			return Y * RenderResolution + X;
		};

		for (int32 Y = 0; Y < RenderResolution; ++Y)
		{
			const float V = RenderResolution > 1
				? static_cast<float>(Y) / static_cast<float>(RenderResolution - 1)
				: 0.0f;

			for (int32 X = 0; X < RenderResolution; ++X)
			{
				const float U = RenderResolution > 1
					? static_cast<float>(X) / static_cast<float>(RenderResolution - 1)
					: 0.0f;

				const float Height01 = FMath::Clamp(HeightSampler(U, V), 0.0f, 1.0f);
				const FVector Position = Mapping.ToPreviewPosition(U, V, Height01);

				const int32 VertexId = Mesh.AppendVertex(
					FVector3d(
						static_cast<double>(Position.X),
						static_cast<double>(Position.Y),
						static_cast<double>(Position.Z)));

				VertexIds[VertexIndex(X, Y)] = VertexId;
			}
		}

		for (int32 Y = 0; Y < RenderResolution - 1; ++Y)
		{
			for (int32 X = 0; X < RenderResolution - 1; ++X)
			{
				const int32 V00 = VertexIds[VertexIndex(X, Y)];
				const int32 V10 = VertexIds[VertexIndex(X + 1, Y)];
				const int32 V01 = VertexIds[VertexIndex(X, Y + 1)];
				const int32 V11 = VertexIds[VertexIndex(X + 1, Y + 1)];

				Mesh.AppendTriangle(V00, V11, V10, 0);
				Mesh.AppendTriangle(V00, V01, V11, 0);
			}
		}

		Mesh.EnableAttributes();

		UE::Geometry::FMeshNormals::QuickComputeVertexNormals(Mesh, false);

		UE::Geometry::FMeshNormals Normals(&Mesh);
		Normals.ComputeVertexNormals(true, true);
		Normals.CopyToOverlay(Mesh.Attributes()->PrimaryNormals(), false);

		OutMesh = MoveTemp(Mesh);
		return true;
	}

	bool BuildDynamicMeshFromLayer(const FUTKLayer& Layer, const FUTKPreviewTerrainMapping& Mapping, UE::Geometry::FDynamicMesh3& OutMesh)
	{
		const FUTKBuffer2D& Buffer = Layer.Data.Get();

		if (!Buffer.IsValid())
			return false;

		const int32 RenderResolution = ComputeDynamicMeshResolution(Buffer, Mapping);

		return BuildDynamicMeshGrid(
			RenderResolution,
			Mapping,
			[&Buffer](float U, float V){
				return SampleLayerNearest(Buffer, U, V);
			},
			OutMesh);
	}

	bool BuildFlatDynamicMesh(const FUTKPreviewTerrainMapping& Mapping, UE::Geometry::FDynamicMesh3& OutMesh)
	{
		const int32 RenderResolution = ComputeDynamicMeshResolution(Mapping);

		return BuildDynamicMeshGrid(
			RenderResolution,
			Mapping,
			[](float, float){
				return 0.0f;
			},
			OutMesh);
	}
}

UUTKTerrainPreviewComponent::UUTKTerrainPreviewComponent()
{
	SetMobility(EComponentMobility::Movable);
	SetVisibility(false, true);
}

void UUTKTerrainPreviewComponent::ClearPreview()
{
	ClearRenderBackend();
	SetVisibility(false, true);

	bHasValidPreview = false;
	CurrentLayerName = NAME_None;
	CurrentMapping = FUTKPreviewTerrainMapping();
	ActiveBackendType = EUTKPreviewBackend::None;
}

void UUTKTerrainPreviewComponent::UpdateFromTerrain(
	const FUTKTerrain& Terrain,
	FName LayerName,
	const FUTKPreviewTerrainMapping& Mapping)
{
	CurrentLayerName = LayerName;
	CurrentMapping = Mapping;
	CurrentMapping.RefreshDerivedValues();

	if (LayerName.IsNone())
	{
		ClearPreview();
		return;
	}

	const bool bUpdated = UpdateDynamicMeshBackend(Terrain, LayerName, CurrentMapping);

	if (!bUpdated)
	{
		ClearPreview();
		return;
	}

	bHasValidPreview = true;
	ActiveBackendType = EUTKPreviewBackend::DynamicMesh;

	SetVisibility(true, false);
}

void UUTKTerrainPreviewComponent::UpdateFlatPreview(const FUTKPreviewTerrainMapping& Mapping)
{
	CurrentLayerName = NAME_None;
	CurrentMapping = Mapping;
	CurrentMapping.RefreshDerivedValues();

	const bool bUpdated = UpdateFlatDynamicMeshBeckend(CurrentMapping);

	if (!bUpdated)
	{
		ClearPreview();
		return;
	}

	bHasValidPreview = true;
	ActiveBackendType = EUTKPreviewBackend::DynamicMesh;

	SetVisibility(true, false);
}

FBoxSphereBounds UUTKTerrainPreviewComponent::GetPreviewBounds() const
{
	if (!bHasValidPreview)
		return FBoxSphereBounds(GetComponentLocation(), FVector::ZeroVector, 0.0f);

	if (const UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(ActiveRenderComponent))
	{
		if (PrimitiveComponent->IsVisible())
			return PrimitiveComponent->Bounds;
	}

	const float HalfFootprintUU = FMath::Max(0.0f, CurrentMapping.PreviewFootprintUU) * 0.5f;
	const float PreviewHeightUU = FMath::Max(0.0f, CurrentMapping.PreviewFootprintUU * CurrentMapping.HeightScaleRatio);

	const FVector LocalOrigin(0.0f, 0.0f, PreviewHeightUU * 0.5f);
	const FVector LocalExtent(HalfFootprintUU, HalfFootprintUU, PreviewHeightUU * 0.5f);

	return FBoxSphereBounds(LocalOrigin, LocalExtent, LocalExtent.Size()).TransformBy(GetComponentTransform());
}

void UUTKTerrainPreviewComponent::ClearRenderBackend()
{
	if (!ActiveRenderComponent)
		return;

	ActiveRenderComponent->SetVisibility(false, true);

	if (UDynamicMeshComponent* DynamicMeshComponent = Cast<UDynamicMeshComponent>(ActiveRenderComponent))
		DynamicMeshComponent->SetMesh(UE::Geometry::FDynamicMesh3());
}

bool UUTKTerrainPreviewComponent::UpdateDynamicMeshBackend(const FUTKTerrain& Terrain, FName LayerName, const FUTKPreviewTerrainMapping& Mapping)
{
	const FUTKLayer* Layer = Terrain.FindLayer(LayerName);
	if (!Layer || !Layer->Data->IsValid())
		return false;

	USceneComponent* RenderComponent = EnsureDynamicMeshRenderComponent();
	if (!RenderComponent)
		return false;

	UDynamicMeshComponent* DynamicMeshComponent = Cast<UDynamicMeshComponent>(RenderComponent);
	if (!DynamicMeshComponent)
		return false;

	UE::Geometry::FDynamicMesh3 Mesh;
	if (!BuildDynamicMeshFromLayer(*Layer, Mapping, Mesh))
		return false;

	DynamicMeshComponent->SetMesh(MoveTemp(Mesh));
	DynamicMeshComponent->SetVisibility(true, true);

	ActiveRenderComponent = RenderComponent;
	return true;
}

bool UUTKTerrainPreviewComponent::UpdateFlatDynamicMeshBeckend(const FUTKPreviewTerrainMapping& Mapping)
{
	USceneComponent* RenderComponent = EnsureDynamicMeshRenderComponent();
	if (!RenderComponent)
		return false;

	UDynamicMeshComponent* DynamicMeshComponent = Cast<UDynamicMeshComponent>(RenderComponent);
	if (!DynamicMeshComponent)
		return false;

	UE::Geometry::FDynamicMesh3 Mesh;
	if (!BuildFlatDynamicMesh(Mapping, Mesh))
		return false;

	DynamicMeshComponent->SetMesh(MoveTemp(Mesh));
	DynamicMeshComponent->SetVisibility(true, true);

	ActiveRenderComponent = RenderComponent;
	return true;
}

USceneComponent* UUTKTerrainPreviewComponent::EnsureDynamicMeshRenderComponent()
{
	if (ActiveRenderComponent && ActiveRenderComponent->IsA<UDynamicMeshComponent>())
		return ActiveRenderComponent;

	AActor* Owner = GetOwner();
	if (!Owner)
		return nullptr;

	UWorld* World = GetWorld();
	if (!World)
		return nullptr;

	const FName ComponentName = MakeUniqueObjectName(
		Owner,
		UDynamicMeshComponent::StaticClass(),
		TEXT("UTKDynamicTerrainPreview"));

	UDynamicMeshComponent* DynamicMeshComponent = NewObject<UDynamicMeshComponent>(
		Owner,
		ComponentName,
		RF_Transient);

	if (!DynamicMeshComponent)
		return nullptr;

	DynamicMeshComponent->SetMobility(EComponentMobility::Movable);
	DynamicMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DynamicMeshComponent->SetGenerateOverlapEvents(false);
	DynamicMeshComponent->SetCastShadow(false);
	DynamicMeshComponent->SetVisibility(false, true);
	DynamicMeshComponent->SetMaterial(0, UMaterial::GetDefaultMaterial(MD_Surface));

	Owner->AddInstanceComponent(DynamicMeshComponent);

	DynamicMeshComponent->AttachToComponent(
		this,
		FAttachmentTransformRules::KeepRelativeTransform);

	DynamicMeshComponent->RegisterComponent();

	ActiveRenderComponent = DynamicMeshComponent;
	return ActiveRenderComponent;
}