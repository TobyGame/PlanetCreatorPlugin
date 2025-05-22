#pragma once

#include "CoreMinimal.h"
#include "UObject/NameTypes.h"
#include "TerrainGraphSaveData.generated.h"


UCLASS()
class TERRAINGENERATIONTOOL_API UTerrainGraphPinSaveData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FName Name;

	UPROPERTY()
	bool bInput = true;

	UPROPERTY()
	FName ConnectedToNode;

	UPROPERTY()
	FName ConnectedPin;
};

UCLASS()
class TERRAINGENERATIONTOOL_API UTerrainGraphNodeSaveData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FName Id;

	UPROPERTY()
	FString NodeType;

	UPROPERTY()
	FVector2D Position;

	UPROPERTY()
	TArray<UTerrainGraphPinSaveData*> Pins;
};

UCLASS()
class TERRAINGENERATIONTOOL_API UTerrainGraphSaveData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<UTerrainGraphNodeSaveData*> Nodes;
};