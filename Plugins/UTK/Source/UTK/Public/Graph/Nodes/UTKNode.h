#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "UTKNodeDefinition.h"
#include "Core/UTKNodeDiagnostics.h"
#include "UTKNodeSettings.h"
#include "UTKNodeExecutionTypes.h"
#include "UTKNode.generated.h"

struct FUTKNodeCacheEntry
{
	TSharedPtr<FUTKTerrain> Terrain;

	int32 CachedResolutionX = 0;
	int32 CachedResolutionY = 0;

	uint64 CachedGraphRevision = 0;
	uint64 CachedPreviewRevision = 0;

	bool IsValidFor(const FUTKNodeExecutionContext& Ctx) const
	{
		if (!Terrain.IsValid())
			return false;

		if (CachedResolutionX != Ctx.ResolutionX ||
			CachedResolutionY != Ctx.ResolutionY)
			return false;

		if (CachedGraphRevision != Ctx.GraphRevision ||
			CachedPreviewRevision != Ctx.PreviewRevision)
			return false;

		return true;
	}
};

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

	void InvalidateCache();
	void InvalidateOutputCache(FName OutputPinName);

	FUTKNodeCacheEntry* FindCacheEntry(FName OutputPinName);
	const FUTKNodeCacheEntry* FindCacheEntry(FName OutputPinName) const;
	FUTKNodeCacheEntry& GetOrAddCacheEntry(FName OutputPinName);

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

	TMap<FName, FUTKNodeCacheEntry> OutputCache;

	UPROPERTY(EditAnywhere, Instanced, Category="UTK | Node")
	TObjectPtr<UUTKNodeSettings> Settings;
};