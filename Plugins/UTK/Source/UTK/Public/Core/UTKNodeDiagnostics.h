#pragma once

#include "CoreMinimal.h"

struct FUTKNodeDiagnostics
{
	double LastEvaluationTime = 0.0f;
	FString Message;
	bool bHasError = false;

	void SetMessage(const FString& InMessage, bool bError = false)
	{
		Message = InMessage;
		bHasError = bError;
	}
};