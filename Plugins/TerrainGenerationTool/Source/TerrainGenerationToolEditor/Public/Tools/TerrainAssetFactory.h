#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "TerrainAssetFactory.generated.h"

UCLASS()
class TERRAINGENERATIONTOOLEDITOR_API UTerrainAssetFactory : public UFactory
{
	GENERATED_BODY()

public:
	UTerrainAssetFactory();

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};