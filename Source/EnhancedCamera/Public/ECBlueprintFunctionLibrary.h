// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ECRTSCamera.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ECBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ENHANCEDCAMERA_API UECBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Enhanced Camera")
	static AECRTSCamera* GetEnhancedCamera();
};
