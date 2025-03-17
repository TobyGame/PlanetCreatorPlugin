#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "PlanetTerrainComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PLANETARYTOOLSRUNTIME_API UPlanetTerrainComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:
	UPlanetTerrainComponent(const FObjectInitializer& ObjectInitializer);

	// Generates a cube-mapped sphere
	void GenerateCubeMapSphere(float Radius, int32 Resolution);

	// Retrieves cached data
	void GetCachedMeshData(TArray<FVector>& OutVertices, TArray<int32>& OutTriangles, TArray<FVector>& OutNormals, TArray<FVector2D>& OutUVs);

	// Creates a static mesh from cached data
	UStaticMesh* CreateStaticMesh();

private:
	// Generates a single cube face
	void GenerateFaceMesh(FVector FaceNormal, float Radius, int32 Resolution, int32 SectionIndex);

	// Caches to avoid redundant calculations
	TArray<FVector> CachedVertices;
	TArray<int32> CachedTriangles;
	TArray<FVector> CachedNormals;
	TArray<FVector2D> CachedUVs;

	FVector ProjectToSphere(float X, float Y, float Z);
};
