#include "Graph/Nodes/NodeProperties/UTKNodePropertyProxy.h"

#include "Graph/Nodes/UTKNode.h"
#include "Graph/Nodes/NodeProperties/UTKPropertyTypes.h"
#include "Logging/LogMacros.h"

void UUTKNodePropertyProxy::InitializeFromNode(UUTKNode* InNode)
{
	TargetNode = InNode;
	Properties.Reset();

	if (TargetNode)
	{
		Properties = TargetNode->GetRuntimeProperties();
	}
}

void UUTKNodePropertyProxy::ApplyToNode()
{
	if (!TargetNode)
		return;

	TargetNode->Modify();
	TargetNode->GetRuntimeProperties() = Properties;
}

void UUTKNodePropertyProxy::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	ApplyToNode();
}

void UUTKNodePropertyProxy::PrintConstantValue()
{
	if (!TargetNode)
		return;

	const FUTKFloatProperty Prop = TargetNode->GetProperty<FUTKFloatProperty>("Value");
	const float Value = Prop.Value;

	UE_LOG(LogTemp,
		Log,
		TEXT("UTK Constant node '%s' value: %f"),
		*TargetNode->GetName(),
		Value);
}