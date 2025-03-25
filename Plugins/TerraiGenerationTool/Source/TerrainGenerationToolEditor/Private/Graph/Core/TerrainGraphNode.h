#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "TerrainGraphNode.generated.h"

UENUM()
enum class ETerrainValueType
{
	None,
	Float,
	HeightMap,
	Mask,
	Vector2D
};

USTRUCT()
struct FTerrainPinDescriptor
{
	GENERATED_BODY()

	UPROPERTY() FName PinName;
	UPROPERTY() ETerrainValueType DataType = ETerrainValueType::None;
	UPROPERTY() int32 Index = -1;
	UPROPERTY() bool bIsInput = false;
	UPROPERTY() bool bIsOptional = false;
	UPROPERTY() bool bIsMainInput = false;
	UPROPERTY() bool bIsPreviewable = true;
};

UCLASS()
class UTerrainGraphNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
	UTerrainGraphNode();

	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FString GetCategoryName() const;

protected:
	virtual void DefinePins(TArray<FTerrainPinDescriptor>& OutPins) {}

public:
	UPROPERTY() TArray<FTerrainPinDescriptor> PinDescriptions;
	UPROPERTY() FText NodeDisplayName;
	UPROPERTY() FString CategoryName;
	UPROPERTY() bool bIsCached = false;
};