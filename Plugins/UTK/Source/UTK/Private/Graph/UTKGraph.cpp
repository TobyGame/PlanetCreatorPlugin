#include "Graph/UTKGraph.h"
#include "Graph/UTKGraphSchema.h"

UUTKGraph::UUTKGraph()
{
	Schema = UUTKGraphSchema::StaticClass();
}


void UUTKGraph::PostLoad()
{
	Super::PostLoad();

	if (!Schema)
	{
		Schema = UUTKGraphSchema::StaticClass();
	}
}