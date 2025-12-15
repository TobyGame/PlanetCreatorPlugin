#include "Graph/UTKGraphEvaluation.h"
#include "Graph/Nodes/UTKNode.h"


bool ResolveInputsForNode(const UUTKNode* Node, TArray<FUTKResolvedInput>& OutInputs)
{
	OutInputs.Reset();

	if (!Node)
		return false;

	const FUTKNodeDefinition& Definition = Node->GetDefinition();
	const TArray<FUTKNodePinDefinition>& PinDefs = Definition.Pins;

	bool bAllRequiredSatisfied = true;

	// We iterate over the logical pin definitions, not raw UEdGraph pins, so that
	// the input ordering is stable and controlled by the node definition.
	for (const FUTKNodePinDefinition& PinDef : PinDefs)
	{
		if (!PinDef.bInput)
			continue;

		FUTKResolvedInput Resolved;
		Resolved.InputPinName = PinDef.Name;
		Resolved.DefaultLayerName = PinDef.DefaultLayerName;
		Resolved.bRequired = PinDef.bRequired;

		// Find the actual graph pin that corresponds to this logical input.
		// UEdGrapNode::FindPin has a const overload, so we can call it on a const node
		if (UEdGraphPin* const InputGraphPin = Node->FindPin(PinDef.Name, EGPD_Input))
		{
			if (InputGraphPin->LinkedTo.Num() > 0)
			{
				if (UEdGraphPin* const LinkedPin = InputGraphPin->LinkedTo[0])
				{
					if (UEdGraphNode* const OwingNode = LinkedPin->GetOwningNode())
					{
						if (UUTKNode* const UpstreamNode = Cast<UUTKNode>(OwingNode))
						{
							Resolved.UpstreamNode = UpstreamNode;
							Resolved.UpstreamOutputPinName = LinkedPin->PinName;

							const FUTKNodeDefinition& UpstreamDef = UpstreamNode->GetDefinition();
							for (const FUTKNodePinDefinition& UpPinDef : UpstreamDef.Pins)
							{
								if (!UpPinDef.bInput && UpPinDef.Name == Resolved.UpstreamOutputPinName)
								{
									if (!UpPinDef.DefaultLayerName.IsNone())
										Resolved.DefaultLayerName = UpPinDef.DefaultLayerName;
									break;
								}
							}
						}
					}
				}
			}
		}

		if (Resolved.bRequired && !Resolved.UpstreamNode)
			bAllRequiredSatisfied = false;

		OutInputs.Add(Resolved);
	}

	return bAllRequiredSatisfied;
}

namespace
{
	TSharedPtr<FUTKTerrain> EvaluateNodeOutput_Internal(UUTKNode* Node, FName OutputPinName, FUTKNodeExecutionContext& Ctx, TSet<FGuid>& EvaluationStack)
	{
		if (!Node)
			return nullptr;

		FUTKNodeDiagnostics& Diagnostics = Node->AccessDiagnostics();
		Diagnostics.bHasError = false;
		Diagnostics.Message.Empty();

		if (EvaluationStack.Contains(Node->NodeGuid))
		{
			Diagnostics.SetMessage(TEXT("Cycle detected in graph"), true);
			return nullptr;
		}

		EvaluationStack.Add(Node->NodeGuid);
		auto PopFromStack = [&EvaluationStack, Node](){
			EvaluationStack.Remove(Node->NodeGuid);
		};
		ON_SCOPE_EXIT{
			PopFromStack();
		};

		if (FUTKNodeCacheEntry* Cache = Node->FindCacheEntry(OutputPinName))
		{
			if (Cache->IsValidFor(Ctx) && Cache->Terrain.IsValid())
			{
				Diagnostics.SetMessage(TEXT("Cached"), false);
				return Cache->Terrain;
			}
		}

		const FUTKNodeDefinition& Definition = Node->GetDefinition();

		TArray<FUTKResolvedInput> ResolvedInputs;
		const bool bAllRequiredInputsSatisfied = ResolveInputsForNode(Node, ResolvedInputs);

		TArray<FUTKNodeInput> Inputs;
		Inputs.Reserve(ResolvedInputs.Num());

		for (const FUTKResolvedInput& Resolved : ResolvedInputs)
		{
			TSharedPtr<FUTKTerrain> UpstreamTerrain;

			if (Resolved.UpstreamNode)
			{
				UpstreamTerrain = EvaluateNodeOutput_Internal(
					Resolved.UpstreamNode,
					Resolved.UpstreamOutputPinName,
					Ctx,
					EvaluationStack);
			}

			Inputs.Emplace(UpstreamTerrain, Resolved.DefaultLayerName);
		}

		int32 NumOutputs = 0;
		for (const FUTKNodePinDefinition& PinDef : Definition.Pins)
		{
			if (!PinDef.bInput)
				++NumOutputs;
		}

		TArray<FUTKNodeOutput> Outputs;
		Outputs.Reserve(NumOutputs);

		for (const FUTKNodePinDefinition& PinDef : Definition.Pins)
		{
			if (!PinDef.bInput)
				Outputs.Emplace(TSharedPtr<FUTKTerrain>(), PinDef.DefaultLayerName);
		}

		FUTKDomain2D Domain(Ctx.ResolutionX, Ctx.ResolutionY);
		FUTKTerrainWorkspace Workspace(Domain);

		const double StartTime = FPlatformTime::Seconds();

		if (Definition.ProcessFunction)
			Definition.ProcessFunction(Inputs, Outputs, Ctx, Workspace, *Node);

		const double EndTime = FPlatformTime::Seconds();
		Diagnostics.LastEvaluationTime = (EndTime - StartTime) * 1000.0; // ms

		int32 OutputIndex = INDEX_NONE;
		int32 CurrentOutputIdx = 0;

		for (const FUTKNodePinDefinition& PinDef : Definition.Pins)
		{
			if (!PinDef.bInput)
			{
				if (PinDef.Name == OutputPinName)
				{
					OutputIndex = CurrentOutputIdx;
					break;
				}

				++CurrentOutputIdx;
			}
		}

		TSharedPtr<FUTKTerrain> ResultTerrain;

		if (Outputs.IsValidIndex(OutputIndex))
			ResultTerrain = Outputs[OutputIndex].Terrain;

		FUTKNodeCacheEntry& CacheEntry = Node->GetOrAddCacheEntry(OutputPinName);
		CacheEntry.Terrain = ResultTerrain;
		CacheEntry.CachedResolutionX = Ctx.ResolutionX;
		CacheEntry.CachedResolutionY = Ctx.ResolutionY;
		CacheEntry.CachedGraphRevision = Ctx.GraphRevision;
		CacheEntry.CachedPreviewRevision = Ctx.PreviewRevision;

		if (!ResultTerrain.IsValid() || !ResultTerrain->IsValid())
			Diagnostics.SetMessage(TEXT("No output terrain."), true);
		else if (!bAllRequiredInputsSatisfied)
			Diagnostics.SetMessage(TEXT("Missing required input(s)."), true);
		else
			Diagnostics.SetMessage(TEXT("OK"), false);

		return ResultTerrain;
	}
}

TSharedPtr<FUTKTerrain> EvaluateNodeOutput(UUTKNode* Node, FName OutputPinName, FUTKNodeExecutionContext& Ctx)
{
	TSet<FGuid> EvaluationStack;
	return EvaluateNodeOutput_Internal(Node, OutputPinName, Ctx, EvaluationStack);
}