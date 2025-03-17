#include "PlanetTerrainComponent.h"
#include "Engine/StaticMesh.h"
#include "StaticMeshAttributes.h"

UPlanetTerrainComponent::UPlanetTerrainComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UPlanetTerrainComponent::GenerateCubeMapSphere(float Radius, int32 Resolution)
{
    // Clear cached data
    CachedVertices.Empty();
    CachedTriangles.Empty();
    CachedNormals.Empty();
    CachedUVs.Empty();

    // Cube face normals
    FVector Directions[6] = {
        FVector::ForwardVector, FVector::BackwardVector,
        FVector::RightVector, FVector::LeftVector,
        FVector::UpVector, FVector::DownVector
    };

    for (int32 Face = 0; Face < 6; ++Face)
    {
        GenerateFaceMesh(Directions[Face], Radius, Resolution, Face);
    }
}

void UPlanetTerrainComponent::GenerateFaceMesh(FVector FaceNormal, float Radius, int32 Resolution, int32 SectionIndex)
{
    int32 VerticesPerEdge = Resolution + 1;
    float Step = 2.0f / (float)Resolution; // Ensure step is float

    int32 BaseIndex = CachedVertices.Num();

    // Generate vertices for the face
    for (int32 Y = 0; Y <= Resolution; ++Y)
    {
        for (int32 X = 0; X <= Resolution; ++X)
        {
            float PX = -1.0f + Step * X;
            float PY = -1.0f + Step * Y;
            FVector Point = FVector((float)PX, (float)PY, 1.0f) * FaceNormal;
            FVector SpherePoint = ProjectToSphere(Point.X, Point.Y, Point.Z) * Radius;

            CachedVertices.Add(SpherePoint);
            CachedNormals.Add(SpherePoint.GetSafeNormal());
            CachedUVs.Add(FVector2D((float)X / (float)Resolution, (float)Y / (float)Resolution));
        }
    }

    // Generate triangles
    for (int32 Y = 0; Y < Resolution; ++Y)
    {
        for (int32 X = 0; X < Resolution; ++X)
        {
            int32 TopLeft = BaseIndex + Y * VerticesPerEdge + X;
            int32 TopRight = TopLeft + 1;
            int32 BottomLeft = BaseIndex + (Y + 1) * VerticesPerEdge + X;
            int32 BottomRight = BottomLeft + 1;

            CachedTriangles.Add(TopLeft);
            CachedTriangles.Add(BottomRight);
            CachedTriangles.Add(BottomLeft);

            CachedTriangles.Add(TopLeft);
            CachedTriangles.Add(TopRight);
            CachedTriangles.Add(BottomRight);
        }
    }
}


FVector UPlanetTerrainComponent::ProjectToSphere(float X, float Y, float Z)
{
    FVector P = FVector(X, Y, Z).GetSafeNormal();
    return P;
}

void UPlanetTerrainComponent::GetCachedMeshData(TArray<FVector>& OutVertices, TArray<int32>& OutTriangles, TArray<FVector>& OutNormals, TArray<FVector2D>& OutUVs)
{
    OutVertices = CachedVertices;
    OutTriangles = CachedTriangles;
    OutNormals = CachedNormals;
    OutUVs = CachedUVs;
}

UStaticMesh* UPlanetTerrainComponent::CreateStaticMesh()
{
    if (CachedVertices.Num() == 0 || CachedTriangles.Num() == 0)
    {
        return nullptr;
    }

    FMeshDescription MeshDescription;
    FStaticMeshAttributes Attributes(MeshDescription);
    Attributes.Register();

    TMap<int32, FVertexID> VertIDMap;

    // Add vertices
    for (int32 i = 0; i < CachedVertices.Num(); ++i)
    {
        FVertexID VertexID = MeshDescription.CreateVertex();
        Attributes.GetVertexPositions()[VertexID] = FVector3f(CachedVertices[i]);
        VertIDMap.Add(i, VertexID);
    }

    // Add triangles
    FPolygonGroupID PolygonGroup = MeshDescription.CreatePolygonGroup();
    for (int32 i = 0; i < CachedTriangles.Num(); i += 3)
    {
        FVertexInstanceID V0 = MeshDescription.CreateVertexInstance(VertIDMap[CachedTriangles[i]]);
        FVertexInstanceID V1 = MeshDescription.CreateVertexInstance(VertIDMap[CachedTriangles[i + 1]]);
        FVertexInstanceID V2 = MeshDescription.CreateVertexInstance(VertIDMap[CachedTriangles[i + 2]]);
        MeshDescription.CreatePolygon(PolygonGroup, { V0, V1, V2 });
    }

    UStaticMesh* StaticMesh = NewObject<UStaticMesh>(GetTransientPackage(), NAME_None, RF_Transient);
    StaticMesh->SetNumSourceModels(1);

    StaticMesh->CreateMeshDescription(0, MeshDescription);
    StaticMesh->CommitMeshDescription(0);

    TArray<const FMeshDescription*> MeshDescriptions;
    MeshDescriptions.Add(&MeshDescription);

    StaticMesh->BuildFromMeshDescriptions(MeshDescriptions);

    FStaticMeshSourceModel& SourceModel = StaticMesh->GetSourceModel(0);
    SourceModel.BuildSettings.bRecomputeNormals = false;
    SourceModel.BuildSettings.bRecomputeTangents = true;

    return StaticMesh;
}

