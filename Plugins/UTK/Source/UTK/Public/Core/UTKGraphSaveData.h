#pragma once

#include "CoreMinimal.h"
#include "UObject/NameTypes.h"
#include "UTKGraphSaveData.generated.h"


UCLASS()
class UTK_API UUTKGraphPinSaveData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FName Name;

	UPROPERTY()
	bool bInput = true;

	UPROPERTY()
	TArray<FGuid> ConnectedToNodes;

	UPROPERTY()
	TArray<FName> ConnectedPins;
};

UCLASS()
class UTK_API UUTKGraphNodeSaveData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FGuid Guid;

	UPROPERTY()
	FString NodeType;

	UPROPERTY()
	FVector2D Position;

	UPROPERTY()
	TArray<UUTKGraphPinSaveData*> Pins;
};

UCLASS()
class UTK_API UUTKGraphSaveData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<UUTKGraphNodeSaveData*> Nodes;
};