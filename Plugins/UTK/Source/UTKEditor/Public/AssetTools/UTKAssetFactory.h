#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "UTKAssetFactory.generated.h"

/**
 * UUTKAssetFactory - Factory for creating terrain assets
 */

UCLASS()
class UTKEDITOR_API UUTKAssetFactory : public UFactory
{
	GENERATED_BODY()

public:
	UUTKAssetFactory();

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual uint32 GetMenuCategories() const override;
	virtual bool ShouldShowInNewMenu() const override;
};