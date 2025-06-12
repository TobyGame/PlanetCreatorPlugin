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
	FName ConnectedToNode;

	UPROPERTY()
	FName ConnectedPin;
};

UCLASS()
class UTK_API UUTKGraphNodeSaveData : public UObject
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