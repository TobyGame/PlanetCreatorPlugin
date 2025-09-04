#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "UTKGraph.generated.h"

UCLASS()
class UUTKGraph : public UEdGraph
{
	GENERATED_BODY()

public:
	UUTKGraph();

	virtual void PostLoad() override;
};