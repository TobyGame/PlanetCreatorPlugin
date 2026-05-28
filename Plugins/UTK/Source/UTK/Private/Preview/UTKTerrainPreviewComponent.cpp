#include "Preview/UTKTerrainPreviewComponent.h"

#include "Components/DynamicMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Core/UTKTerrainTypes.h"
#include "DynamicMesh/DynamicMesh3.h"
#include "DynamicMesh/MeshNormals.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "GameFramework/Actor.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInterface.h"


namespace
{
	constexpr int32 UTKPreviewMinDynamicMeshResolution = 2;
	constexpr int32 UTKPreviewMaxDynamicMeshResolution = 4096;

	const FName UTKHeightTextureParameterName(TEXT("UTK_HeightTexture"));

	bool IsNaniteHeightTextureConfigurationValid(
		const FUTKPreviewTerrainMapping& Mapping)
	{
		UStaticMesh* PreviewMesh = Mapping.NanitePreviewMesh.Get();
		if (!PreviewMesh)
		{
			UE_LOG(LogTemp, Warning, TEXT("[UTK] Nanite Height Texture backend requested, but no Nanite preview mesh is assigned."));
			return false;
		}

		if (!Mapping.NaniteDisplacementMaterial.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("[UTK] Nanite Height Texture backend requested, but no Nanite displacement material is assigned."));
			return false;
		}

		if (!PreviewMesh->NaniteSettings.bEnabled)
		{
			UE_LOG(LogTemp,
				Warning,
				TEXT("[UTK] Nanite Height Texture backend requested, but preview mesh '%s' does not have Nanite enabled. Falling back to DynamicMesh."),
				*PreviewMesh->GetName());

			return false;
		}

		return true;
	}

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

	void EncodeHeightToBGRA8(const FUTKBuffer2D& Buffer, int32 Width, int32 Height, uint8* PixelBytes)
	{
		constexpr int32 BytesPerPixel = 4;

		for (int32 Y = 0; Y < Height; ++Y)
		{
			const float V = Height > 1
				? static_cast<float>(Y) / static_cast<float>(Height - 1)
				: 0.0f;

			for (int32 X = 0; X < Width; ++X)
			{
				const float U = Width > 1
					? static_cast<float>(X) / static_cast<float>(Width - 1)
					: 0.0f;

				const float Height01 = FMath::Clamp(SampleLayerNearest(Buffer, U, V), 0.0f, 1.0f);
				const uint8 EncodedHeight = static_cast<uint8>(FMath::Clamp(FMath::RoundToInt(Height01 * 255.0f), 0, 255));

				const int32 PixelOffset = (Y * Width + X) * BytesPerPixel;

				// PF_B8G8R8A8 memory order
				PixelBytes[PixelOffset + 0] = EncodedHeight;
				PixelBytes[PixelOffset + 1] = EncodedHeight;
				PixelBytes[PixelOffset + 2] = EncodedHeight;
				PixelBytes[PixelOffset + 3] = 255;
			}
		}
	}

	void EncodeFlatHeightToBGRA8(int32 Width, int32 Height, uint8* PixelBytes)
	{
		constexpr int32 BytesPerPixel = 4;
		const int32 NumBytes = Width * Height * BytesPerPixel;

		FMemory::Memzero(PixelBytes, NumBytes);

		for (int32 PixelIndex = 0; PixelIndex < Width * Height; ++PixelIndex)
			PixelBytes[PixelIndex * BytesPerPixel + 3] = 255;
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

	HeightTexture = nullptr;
	NanitePreviewMaterialInstance = nullptr;
	NaniteDisplacementParentMaterial = nullptr;
	LastAppliedHeightTexture = nullptr;
	CurrentNanitePreviewMesh = nullptr;

	HeightTextureHeight = 0;
	HeightTextureWidth = 0;
	LastAppliedNaniteMagnitudeUU = -1.0f;
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
		UpdateFlatPreview(CurrentMapping);
		return;
	}

	const bool bUpdated = UpdateRenderBackend(Terrain, LayerName, CurrentMapping);

	if (!bUpdated)
	{
		UpdateFlatPreview(CurrentMapping);
		return;
	}

	bHasValidPreview = true;
	SetVisibility(true, false);
}

void UUTKTerrainPreviewComponent::UpdateFlatPreview(const FUTKPreviewTerrainMapping& Mapping)
{
	CurrentLayerName = NAME_None;
	CurrentMapping = Mapping;
	CurrentMapping.RefreshDerivedValues();

	const bool bUpdated = UpdateFlatRenderBackend(CurrentMapping);

	if (!bUpdated)
	{
		ClearPreview();
		return;
	}

	bHasValidPreview = true;
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
	if (DynamicMeshComponent)
	{
		DynamicMeshComponent->SetVisibility(false, true);
		DynamicMeshComponent->SetMesh(UE::Geometry::FDynamicMesh3());
	}


	if (NaniteStaticMeshComponent)
		NaniteStaticMeshComponent->SetVisibility(false, true);

	ActiveRenderComponent = nullptr;
}

EUTKPreviewBackend UUTKTerrainPreviewComponent::ResolveSupportedBackend(const FUTKPreviewTerrainMapping& Mapping) const
{
	switch (Mapping.PreferredBackend)
	{
	case EUTKPreviewBackend::None:
		return EUTKPreviewBackend::None;

	case EUTKPreviewBackend::DynamicMesh:
		return EUTKPreviewBackend::DynamicMesh;

	case EUTKPreviewBackend::HeightTexture:
	case EUTKPreviewBackend::ChunkedHeightTexture:
		return IsNaniteHeightTextureConfigurationValid(Mapping)
			? EUTKPreviewBackend::HeightTexture
			: EUTKPreviewBackend::DynamicMesh;
	default:
		return EUTKPreviewBackend::DynamicMesh;
	}
}

bool UUTKTerrainPreviewComponent::UpdateRenderBackend(const FUTKTerrain& Terrain, FName LayerName, const FUTKPreviewTerrainMapping& Mapping)
{
	const EUTKPreviewBackend ResolvedBackend = ResolveSupportedBackend(Mapping);

	switch (ResolvedBackend)
	{
	case EUTKPreviewBackend::None:
		ClearRenderBackend();
		ActiveBackendType = EUTKPreviewBackend::None;
		return false;

	case EUTKPreviewBackend::DynamicMesh:
		if (UpdateDynamicMeshBackend(Terrain, LayerName, Mapping))
		{
			HideNaniteBackend();
			ActiveBackendType = EUTKPreviewBackend::DynamicMesh;
			return true;
		}
		return false;

	case EUTKPreviewBackend::HeightTexture:
		if (UpdateNaniteHeightTextureBackend(Terrain, LayerName, Mapping))
		{
			HideDynamicMeshBackend();
			ActiveBackendType = EUTKPreviewBackend::HeightTexture;
			return true;
		}
		return false;

	case EUTKPreviewBackend::ChunkedHeightTexture:
	default:
		return false;
	}
}

bool UUTKTerrainPreviewComponent::UpdateFlatRenderBackend(const FUTKPreviewTerrainMapping& Mapping)
{
	const EUTKPreviewBackend ResolvedBackend = ResolveSupportedBackend(Mapping);

	switch (ResolvedBackend)
	{
	case EUTKPreviewBackend::None:
		ClearRenderBackend();
		ActiveBackendType = EUTKPreviewBackend::None;
		return false;

	case EUTKPreviewBackend::DynamicMesh:
		if (UpdateFlatDynamicMeshBackend(Mapping))
		{
			HideNaniteBackend();
			ActiveBackendType = EUTKPreviewBackend::DynamicMesh;
			return true;
		}
		return false;

	case EUTKPreviewBackend::HeightTexture:
		if (UpdateFlatNaniteHeightTextureBackend(Mapping))
		{
			HideDynamicMeshBackend();
			ActiveBackendType = EUTKPreviewBackend::HeightTexture;
			return true;
		}
		return false;

	case EUTKPreviewBackend::ChunkedHeightTexture:
	default:
		return false;
	}
}

bool UUTKTerrainPreviewComponent::UpdateDynamicMeshBackend(const FUTKTerrain& Terrain, FName LayerName, const FUTKPreviewTerrainMapping& Mapping)
{
	const FUTKLayer* Layer = Terrain.FindLayer(LayerName);
	if (!Layer || !Layer->Data->IsValid())
		return false;

	UDynamicMeshComponent* RenderComponent = EnsureDynamicMeshRenderComponent();
	if (!RenderComponent)
		return false;

	UE::Geometry::FDynamicMesh3 Mesh;
	if (!BuildDynamicMeshFromLayer(*Layer, Mapping, Mesh))
		return false;

	RenderComponent->SetMesh(MoveTemp(Mesh));
	RenderComponent->SetVisibility(true, true);

	ActiveRenderComponent = RenderComponent;
	return true;
}

bool UUTKTerrainPreviewComponent::UpdateFlatDynamicMeshBackend(const FUTKPreviewTerrainMapping& Mapping)
{
	UDynamicMeshComponent* RenderComponent = EnsureDynamicMeshRenderComponent();
	if (!RenderComponent)
		return false;

	UE::Geometry::FDynamicMesh3 Mesh;
	if (!BuildFlatDynamicMesh(Mapping, Mesh))
		return false;

	RenderComponent->SetMesh(MoveTemp(Mesh));
	RenderComponent->SetVisibility(true, true);

	ActiveRenderComponent = RenderComponent;
	return true;
}

bool UUTKTerrainPreviewComponent::UpdateNaniteHeightTextureBackend(const FUTKTerrain& Terrain, FName LayerName, const FUTKPreviewTerrainMapping& Mapping)
{
	const FUTKLayer* Layer = Terrain.FindLayer(LayerName);
	if (!Layer || !Layer->Data->IsValid())
		return false;

	if (!UpdateNanitePreviewMesh(Mapping))
		return false;

	if (!UpdateHeightTextureFromLayer(*Layer, Mapping))
		return false;

	if (!ApplyNaniteDisplacementMaterial(Mapping))
		return false;

	if (!NaniteStaticMeshComponent)
		return false;

	NaniteStaticMeshComponent->SetVisibility(true, true);
	ActiveRenderComponent = NaniteStaticMeshComponent;
	return true;
}

bool UUTKTerrainPreviewComponent::UpdateFlatNaniteHeightTextureBackend(const FUTKPreviewTerrainMapping& Mapping)
{
	if (!UpdateNanitePreviewMesh(Mapping))
		return false;

	if (!UpdateFlatHeightTexture(Mapping))
		return false;

	if (!ApplyNaniteDisplacementMaterial(Mapping))
		return false;

	if (!NaniteStaticMeshComponent)
		return false;

	NaniteStaticMeshComponent->SetVisibility(true, true);
	ActiveRenderComponent = NaniteStaticMeshComponent;
	return true;
}

bool UUTKTerrainPreviewComponent::UpdateNanitePreviewMesh(const FUTKPreviewTerrainMapping& Mapping)
{
	UStaticMesh* PreviewMesh = Mapping.NanitePreviewMesh.Get();
	if (!PreviewMesh)
		return false;

	UStaticMeshComponent* StaticMeshComponent = EnsureNaniteStaticMeshComponent();
	if (!StaticMeshComponent)
		return false;

	if (CurrentNanitePreviewMesh != PreviewMesh)
	{
		StaticMeshComponent->SetStaticMesh(PreviewMesh);
		CurrentNanitePreviewMesh = PreviewMesh;
	}

	StaticMeshComponent->SetRelativeLocation(FVector::ZeroVector);
	StaticMeshComponent->SetRelativeRotation(FRotator::ZeroRotator);
	StaticMeshComponent->SetRelativeScale3D(FVector::OneVector);

	StaticMeshComponent->SetBoundsScale(1.0f);

	return true;
}

bool UUTKTerrainPreviewComponent::UpdateHeightTextureFromLayer(const FUTKLayer& Layer, const FUTKPreviewTerrainMapping& Mapping)
{
	const FUTKBuffer2D& Buffer = Layer.Data.Get();
	if (!Buffer.IsValid())
		return false;

	const int32 TargetResolution = FMath::Clamp(Mapping.Resolution > 0 ? Mapping.Resolution : FMath::Min(Buffer.Width, Buffer.Height), 2, 4096);

	const int32 Width = FMath::Min(TargetResolution, Buffer.Width);
	const int32 Height = FMath::Min(TargetResolution, Buffer.Height);

	UTexture2D* Texture = CreateOrResizeHeightTexture(Width, Height);
	if (!Texture)
		return false;

	constexpr int32 BytesPerPixel = 4;
	const int32 NumBytes = Width * Height * BytesPerPixel;

	uint8* PixelBytes = new uint8[NumBytes];
	EncodeHeightToBGRA8(Buffer, Width, Height, PixelBytes);

	FUpdateTextureRegion2D* Region = new FUpdateTextureRegion2D(0, 0, 0, 0, Width, Height);

	Texture->UpdateTextureRegions(0,
		1,
		Region,
		Width * BytesPerPixel,
		BytesPerPixel,
		PixelBytes,
		[](uint8* SrcData, const FUpdateTextureRegion2D* Regions){
			delete[] SrcData;
			delete Regions;
		});

	return true;
}

bool UUTKTerrainPreviewComponent::UpdateFlatHeightTexture(const FUTKPreviewTerrainMapping& Mapping)
{
	const int32 Resolution = FMath::Clamp(Mapping.Resolution > 0 ? Mapping.Resolution : 512, 2, 4096);

	UTexture2D* Texture = CreateOrResizeHeightTexture(Resolution, Resolution);
	if (!Texture)
		return false;

	constexpr int32 BytesPerPixel = 4;
	const int32 NumBytes = Resolution * Resolution * BytesPerPixel;

	uint8* PixelBytes = new uint8[NumBytes];
	EncodeFlatHeightToBGRA8(Resolution, Resolution, PixelBytes);

	FUpdateTextureRegion2D* Region = new FUpdateTextureRegion2D(0, 0, 0, 0, Resolution, Resolution);

	Texture->UpdateTextureRegions(0,
		1,
		Region,
		Resolution * BytesPerPixel,
		BytesPerPixel,
		PixelBytes,
		[](uint8* SrcData, const FUpdateTextureRegion2D* Regions){
			delete[] SrcData;
			delete Regions;
		});

	return true;
}

void UUTKTerrainPreviewComponent::ApplyNaniteBoundsScale(float MagnitudeUU)
{
	if (!NaniteStaticMeshComponent)
		return;

	const float SafeMagnitudeUU = FMath::Max(0.0f, MagnitudeUU);
	const float FootprintUU = FMath::Max(1.0f, CurrentMapping.PreviewFootprintUU);

	const float BoundsScale = FMath::Clamp(
		1.0f + SafeMagnitudeUU / FootprintUU,
		1.0f,
		16.0f);

	NaniteStaticMeshComponent->SetBoundsScale(BoundsScale);
	NaniteStaticMeshComponent->MarkRenderStateDirty();
}

bool UUTKTerrainPreviewComponent::ApplyNaniteDisplacementMaterial(const FUTKPreviewTerrainMapping& Mapping)
{
#if WITH_EDITOR
	if (!NaniteStaticMeshComponent || !HeightTexture)
		return false;

	UMaterialInterface* ParentMaterial = Mapping.NaniteDisplacementMaterial.Get();
	if (!ParentMaterial)
		return false;

	const float MagnitudeUU = FMath::Max(0.0f, Mapping.GetPreviewHeightScaleUU());

	const bool bParentChanged = !NanitePreviewMaterialInstance || NaniteDisplacementParentMaterial != ParentMaterial;

	if (bParentChanged)
	{
		NanitePreviewMaterialInstance = NewObject<UMaterialInstanceConstant>(
			this,
			MakeUniqueObjectName(this, UMaterialInstanceConstant::StaticClass(), TEXT("UTKNaniteHeightTextureMIC")),
			RF_Transient);

		if (!NanitePreviewMaterialInstance)
			return false;

		NanitePreviewMaterialInstance->SetParentEditorOnly(ParentMaterial, true);

		NaniteDisplacementParentMaterial = ParentMaterial;
		LastAppliedHeightTexture = nullptr;
		LastAppliedNaniteMagnitudeUU = -1.0f;
	}

	if (!NanitePreviewMaterialInstance)
		return false;

	bool bMaterialInstanceChanged = bParentChanged;

	if (LastAppliedHeightTexture != HeightTexture)
	{
		NanitePreviewMaterialInstance->SetTextureParameterValueEditorOnly(
			FMaterialParameterInfo(UTKHeightTextureParameterName),
			HeightTexture);

		LastAppliedHeightTexture = HeightTexture;
		bMaterialInstanceChanged = true;
	}

	if (!FMath::IsNearlyEqual(LastAppliedNaniteMagnitudeUU, MagnitudeUU, KINDA_SMALL_NUMBER))
	{
		NanitePreviewMaterialInstance->DisplacementScaling.Magnitude = MagnitudeUU;
		NanitePreviewMaterialInstance->DisplacementScaling.Center = 0.0f;
		NanitePreviewMaterialInstance->bEnableTessellation = true;

		LastAppliedNaniteMagnitudeUU = MagnitudeUU;
		bMaterialInstanceChanged = true;
	}

	if (bMaterialInstanceChanged)
	{
		//NanitePreviewMaterialInstance->PostEditChange();
		NanitePreviewMaterialInstance->UpdateCachedData();
		NanitePreviewMaterialInstance->RecacheUniformExpressions(true);
	}

	NaniteStaticMeshComponent->SetMaterial(0, NanitePreviewMaterialInstance);
	ApplyNaniteBoundsScale(MagnitudeUU);

	NaniteStaticMeshComponent->MarkRenderStateDirty();

	return true;
#else
	return false;
#endif
}

UTexture2D* UUTKTerrainPreviewComponent::CreateOrResizeHeightTexture(int32 Width, int32 Height)
{
	if (Width <= 0 || Height <= 0)
		return nullptr;

	if (HeightTexture && HeightTextureWidth == Width && HeightTextureHeight == Height)
		return HeightTexture;

	HeightTexture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8, TEXT("UTKHeightPreviewTexture"));

	if (!HeightTexture)
		return nullptr;

	HeightTexture->SRGB = false;
	HeightTexture->NeverStream = true;
	HeightTexture->Filter = TF_Bilinear;
	HeightTexture->AddressX = TA_Clamp;
	HeightTexture->AddressY = TA_Clamp;
	HeightTexture->UpdateResource();

	HeightTextureWidth = Width;
	HeightTextureHeight = Height;

	LastAppliedHeightTexture = nullptr;

	return HeightTexture;
}

UDynamicMeshComponent* UUTKTerrainPreviewComponent::EnsureDynamicMeshRenderComponent()
{
	if (DynamicMeshComponent)
		return DynamicMeshComponent;

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

	DynamicMeshComponent = NewObject<UDynamicMeshComponent>(
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

	return DynamicMeshComponent;
}

UStaticMeshComponent* UUTKTerrainPreviewComponent::EnsureNaniteStaticMeshComponent()
{
	if (NaniteStaticMeshComponent)
		return NaniteStaticMeshComponent;

	AActor* Owner = GetOwner();
	if (!Owner)
		return nullptr;

	UWorld* World = GetWorld();
	if (!World)
		return nullptr;

	const FName ComponentName = MakeUniqueObjectName(
		Owner,
		UStaticMeshComponent::StaticClass(),
		TEXT("UTKNaniteTerrainPreview"));

	NaniteStaticMeshComponent = NewObject<UStaticMeshComponent>(
		Owner,
		ComponentName,
		RF_Transient);

	if (!NaniteStaticMeshComponent)
		return nullptr;

	NaniteStaticMeshComponent->SetMobility(EComponentMobility::Movable);
	NaniteStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NaniteStaticMeshComponent->SetGenerateOverlapEvents(false);
	NaniteStaticMeshComponent->SetCastShadow(false);
	NaniteStaticMeshComponent->SetVisibility(false, true);

	Owner->AddInstanceComponent(NaniteStaticMeshComponent);

	NaniteStaticMeshComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

	NaniteStaticMeshComponent->RegisterComponent();

	return NaniteStaticMeshComponent;
}

void UUTKTerrainPreviewComponent::HideDynamicMeshBackend()
{
	if (DynamicMeshComponent)
		DynamicMeshComponent->SetVisibility(false, true);
}

void UUTKTerrainPreviewComponent::HideNaniteBackend()
{
	if (NaniteStaticMeshComponent)
		NaniteStaticMeshComponent->SetVisibility(false, true);
}