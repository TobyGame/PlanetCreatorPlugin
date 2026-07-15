#include "Graph/Nodes/UTK_MathNodes.h"

#include "Graph/Nodes/UTKNodeMacros.h"


/**
 * Temporary validation fixtures.
 * 
 * The implementation is now owned by UTKPrototypeOperators.cpp. These node
 * definitions contain only editor-facing declarations and operator bindings.
 */
UTK_DEFINE_NODE(
	Constant,
	FUTKNodeSpecBuilder(TEXT("Constant"))
	.DisplayName(TEXT("Constant"))
	.Category(TEXT("Internal|Prototypes"))
	.Tooltip(TEXT("Temporary generator fixture for node-system validation."))
	.Settings<UUTKConstantSettings>()
	.Output(TEXT("Result"), EUTKFieldType::Scalar)
	.Operator(TEXT("UTK.Internal.Prototype.Constant"))
	.Internal());

UTK_DEFINE_NODE(
	Combine,
	FUTKNodeSpecBuilder(TEXT("Combine"))
	.DisplayName(TEXT("Combine"))
	.Category(TEXT("Internal|Prototypes"))
	.Tooltip(TEXT("Temporary pointwise fixture for node-system validation."))
	.Settings<UUTKCombineSettings>()
	.Input(TEXT("A"), EUTKFieldType::Scalar)
	.Input(TEXT("B"), EUTKFieldType::Scalar)
	.Output(TEXT("Result"), EUTKFieldType::Scalar)
	.Operator(TEXT("UTK.Internal.Prototype.Lerp"))
	.Internal());

UTK_DEFINE_NODE(
	MultiOutputTest,
	FUTKNodeSpecBuilder(TEXT("MultiOutputTest"))
	.DisplayName(TEXT("Multi Output Test"))
	.Category(TEXT("Internal|Prototypes"))
	.Tooltip(TEXT("Temporary multi-output fixture for node-system validation."))
	.Settings<UUTKMultiOutputTestSettings>()
	.Input(TEXT("In"), EUTKFieldType::Scalar)
	.Output(TEXT("Base"), EUTKFieldType::Scalar)
	.Output(TEXT("Low"), EUTKFieldType::Scalar)
	.Output(TEXT("High"), EUTKFieldType::Scalar)
	.Operator(TEXT("UTK.Internal.Prototype.MultiOutput"))
	.Internal());

void RegisterMathNodes()
{
	REGISTER_UTK_NODE(Constant);
	REGISTER_UTK_NODE(Combine);
	REGISTER_UTK_NODE(MultiOutputTest);
}