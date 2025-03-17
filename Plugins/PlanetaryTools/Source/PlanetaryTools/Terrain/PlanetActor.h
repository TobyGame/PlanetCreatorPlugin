// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlanetActor.generated.h"

UCLASS()
class PLANETARYTOOLSRUNTIME_API APlanetActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APlanetActor();

protected:
	// Called when the game starts or when spawned
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	UPROPERTY(VisibleAnywhere)
	class UPlanetTerrainComponent* PlanetTerrain;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* PlanetMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Planet Config")
	float SphereRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet Config")
	int32 Resolution;
};