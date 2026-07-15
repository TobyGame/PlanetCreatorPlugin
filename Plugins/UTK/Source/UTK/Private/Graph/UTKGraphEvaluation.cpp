#include "Graph/UTKGraphEvaluation.h"

#include "Graph/Nodes/UTKNode.h"
#include "Graph/Operators/UTKOperatorRegistry.h"
#include "Misc/ScopeExit.h"


bool ResolveInputsForNode(const UUTKNode* Node, TArray<FUTKResolvedInput>& OutInputs)
{
	OutInputs.Reset();

	if (!Node)
		return false;

	const FUTKNodeDefinition& Definition = Node->GetDefinition();

	bool bAllRequiredSatisfied = true;

	// We iterate over the logical pin definitions, not raw UEdGraph pins, so that
	// the input ordering is stable and controlled by the node definition.
	for (const FUTKNodePinDefinition& PinDef : Definition.Pins)
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
					if (UUTKNode* const UpstreamNode = Cast<UUTKNode>(LinkedPin->GetOwningNode()))
					{
						Resolved.UpstreamNode = UpstreamNode;
						Resolved.UpstreamOutputPinName = LinkedPin->PinName;

						if (const FUTKNodePinDefinition* UpstreamPin = UpstreamNode->FindPinDefinition(LinkedPin->PinName, EGPD_Output))
							Resolved.DefaultLayerName = UpstreamPin->DefaultLayerName;
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
	void FillCacheEntry(UUTKNode& Node, FName OutputPinName, const TSharedPtr<FUTKTerrain>& Terrain, const FUTKNodeExecutionContext& Context)
	{
		if (!Terrain.IsValid() || !Terrain->IsValid())
			return;

		FUTKNodeCacheEntry& CacheEntry = Node.GetOrAddCacheEntry(OutputPinName);

		CacheEntry.Terrain = Terrain;
		CacheEntry.CachedResolutionX = Context.ResolutionX;
		CacheEntry.CachedResolutionY = Context.ResolutionY;
		CacheEntry.CachedGraphRevision = Context.GraphRevision;
		CacheEntry.CachedPreviewRevision = Context.PreviewRevision;
	}

	TSharedPtr<FUTKTerrain> EvaluateNodeOutput_Internal(UUTKNode* Node, FName OutputPinName, FUTKNodeExecutionContext& Context, TSet<FGuid>& EvaluationStack)
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

		ON_SCOPE_EXIT{
			EvaluationStack.Remove(Node->NodeGuid);
		};

		if (FUTKNodeCacheEntry* Cache = Node->FindCacheEntry(OutputPinName))
		{
			if (Cache->IsValidFor(Context))
			{
				Diagnostics.SetMessage(TEXT("Cached"), false);
				return Cache->Terrain;
			}
		}

		const FUTKNodeDefinition& Definition = Node->GetDefinition();

		const FUTKOperatorDefinition* Operator = FUTKOperatorRegistry::Get().FindOperator(Definition.OperatorId);

		if (!Operator)
		{
			Diagnostics.SetMessage(TEXT("Node operator is not registered."), true);

			return nullptr;
		}

		if (!Operator->ReferenceEvaluator)
		{
			Diagnostics.SetMessage(TEXT("No temporary reference evaluator is registered."), true);

			return nullptr;
		}

		TArray<FUTKResolvedInput> ResolvedInputs;

		if (!ResolveInputsForNode(Node, ResolvedInputs))
		{
			Diagnostics.SetMessage(TEXT("Missing required input(s)."), true);

			return nullptr;
		}

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
					Context,
					EvaluationStack);

				if (!UpstreamTerrain)
				{
					Diagnostics.SetMessage(TEXT("Failed to evaluate an upstream input."), true);

					return nullptr;
				}
			}

			Inputs.Emplace(UpstreamTerrain, Resolved.DefaultLayerName);
		}

		TArray<FUTKNodeOutput> Outputs;
		TArray<FName> OutputPinNames;

		for (const FUTKNodePinDefinition& PinDef : Definition.Pins)
		{
			if (!PinDef.bInput)
			{
				Outputs.Emplace(TSharedPtr<FUTKTerrain>(), PinDef.DefaultLayerName);
				OutputPinNames.Add(PinDef.Name);
			}
		}

		if (Outputs.IsEmpty())
		{
			Diagnostics.SetMessage(TEXT("Node has no output."), true);

			return nullptr;
		}

		int32 NumOutputs = 0;
		for (const FUTKNodePinDefinition& PinDef : Definition.Pins)
		{
			if (!PinDef.bInput)
				++NumOutputs;
		}

		FUTKTerrainWorkspace Workspace(FUTKDomain2D(Context.ResolutionX, Context.ResolutionY));

		const FUTKOperatorReferenceEvaluationRequest Request
		{
			*Node,
			Inputs,
			Context,
			Workspace
		};

		const double StartTime = FPlatformTime::Seconds();

		FString EvaluationError;

		const bool bSuccess = Operator->ReferenceEvaluator(Request, Outputs, EvaluationError);

		Diagnostics.LastEvaluationTime = (FPlatformTime::Seconds() - StartTime) * 1000.0;

		if (!bSuccess)
		{
			Diagnostics.SetMessage(EvaluationError.IsEmpty() ? TEXT("Operator evaluation failed.") : EvaluationError, true);
			return nullptr;
		}

		TSharedPtr<FUTKTerrain> ResultTerrain;

		for (int32 OutputIndex = 0; OutputIndex < Outputs.Num(); ++OutputIndex)
		{
			const TSharedPtr<FUTKTerrain>& OutputTerrain = Outputs[OutputIndex].Terrain;

			FillCacheEntry(*Node, OutputPinNames[OutputIndex], OutputTerrain, Context);

			if (OutputPinNames[OutputIndex] == OutputPinName)
				ResultTerrain = OutputTerrain;
		}

		if (!ResultTerrain.IsValid() || !ResultTerrain->IsValid())
		{
			Diagnostics.SetMessage(TEXT("Requested output was not produced."), true);
			return nullptr;
		}

		Diagnostics.SetMessage(TEXT("OK"), false);
		return ResultTerrain;
	}
}

TSharedPtr<FUTKTerrain> EvaluateNodeOutput(UUTKNode* Node, FName OutputPinName, FUTKNodeExecutionContext& Ctx)
{
	TSet<FGuid> EvaluationStack;
	return EvaluateNodeOutput_Internal(Node, OutputPinName, Ctx, EvaluationStack);
}