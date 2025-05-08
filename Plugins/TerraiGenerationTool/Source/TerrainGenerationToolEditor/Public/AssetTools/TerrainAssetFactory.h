#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "TerrainAssetFactory.generated.h"

/**
 * UTerrainAssetFactory - Factory for creating terrain assets
 */

UCLASS()
class TERRAINGENERATIONTOOLEDITOR_API UTerrainAssetFactory : public UFactory
{
	GENERATED_BODY()

public:
	UTerrainAssetFactory();

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual uint32 GetMenuCategories() const override;
	virtual bool ShouldShowInNewMenu() const override;
};