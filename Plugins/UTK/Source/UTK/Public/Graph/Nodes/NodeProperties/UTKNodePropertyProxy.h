#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "StructUtils/InstancedStruct.h"
#include "UTKNodePropertyProxy.generated.h"

class UUTKNode;

/**
 * Proxy UObject used by the Node Properties tab to edit a UUTKNode's
 * runtime properties via a standard DetailsView.
 * 
 * It mirrors RuntimeProperties from the node into a TMap<FName, FInstancedStruct>,
 * and writes changes back to the node on property edits.
 */
UCLASS()
class UTK_API UUTKNodePropertyProxy : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="UTK")
	TMap<FName, FInstancedStruct> Properties;

	UPROPERTY(Transient)
	TObjectPtr<UUTKNode> TargetNode;

	void InitializeFromNode(UUTKNode* InNode);

	void ApplyToNode();

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	UFUNCTION(CallInEditor, Category="UTK|Debug")
	void PrintConstantValue();
};