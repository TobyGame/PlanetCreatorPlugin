#include "PlanetActor.h"
#include "PlanetTerrainComponent.h"
#include "Async/Async.h"
#include "Engine/StaticMesh.h"

APlanetActor::APlanetActor()
{
	PrimaryActorTick.bCanEverTick = false;

	PlanetTerrain = CreateDefaultSubobject<UPlanetTerrainComponent>(TEXT("PlanetTerrain"));
	RootComponent = PlanetTerrain;

	PlanetMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlanetMesh"));
	PlanetMeshComp->SetupAttachment(RootComponent);

	SphereRadius = 5000.0f;
	Resolution = 8; // Start with low-res
}

void APlanetActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Step 1: Generate and display a low-resolution mesh immediately
	PlanetTerrain->GenerateCubeMapSphere(SphereRadius, Resolution);
	UStaticMesh* LowResMesh = PlanetTerrain->CreateStaticMesh();
	if (LowResMesh)
	{
		PlanetMeshComp->SetStaticMesh(LowResMesh);
	}

	// Step 2: Asynchronously generate the high-resolution mesh in the background
	int32 HighResolution = 32; // Example high-res value
	Async(EAsyncExecution::ThreadPool, [this, HighResolution]()
	{
		PlanetTerrain->GenerateCubeMapSphere(SphereRadius, HighResolution);
		UStaticMesh* HighResMesh = PlanetTerrain->CreateStaticMesh();

		if (HighResMesh)
		{
			// Safely update the static mesh on the game thread
			AsyncTask(ENamedThreads::GameThread, [this, HighResMesh]()
			{
				PlanetMeshComp->SetStaticMesh(HighResMesh);
			});
		}
	});
}
