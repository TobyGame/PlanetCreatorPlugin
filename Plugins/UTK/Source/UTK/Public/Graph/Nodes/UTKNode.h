#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "UTKNodeDefinition.h"
#include "Core/UTKNodeDiagnostics.h"
#include "UTKNodeSettings.h"
#include "UTKNode.generated.h"

UCLASS()
class UUTKNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
	UUTKNode();

	UPROPERTY()
	FName NodeType;

	UPROPERTY()
	TMap<FName, FInstancedStruct> RuntimeProperty;

	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual void PostLoad() override;
	virtual void PostDuplicate(bool bDuplicateForPIE) override;

	void SetDefinition(const FUTKNodeDefinition& InDefinition);
	const FUTKNodeDefinition& GetDefinition() const;

	const FUTKNodeDiagnostics& GetDiagnostics() const { return Diagnostics; }
	FUTKNodeDiagnostics& AccessDiagnostics() { return Diagnostics; }

	UUTKNodeSettings* GetSettings() const { return Settings; }

	template <typename TSettings>
	TSettings* GetSettingsTyped() const
	{
		return Cast<TSettings>(Settings);
	}

	void EnsureSettingsInstance(const FUTKNodeDefinition& Definition);

private:
	void RebuildDefinitionFromType();

	FUTKNodeDefinition NodeDefinition;
	FUTKNodeDiagnostics Diagnostics;

	UPROPERTY(EditAnywhere, Instanced, Category="UTK | Node")
	TObjectPtr<UUTKNodeSettings> Settings;
};