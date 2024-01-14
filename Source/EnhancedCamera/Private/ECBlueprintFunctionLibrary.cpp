// Fill out your copyright notice in the Description page of Project Settings.


#include "ECBlueprintFunctionLibrary.h"

#include "ECRTSCamera.h"
#include "Kismet/GameplayStatics.h"

AECRTSCamera* UECBlueprintFunctionLibrary::GetEnhancedCamera()
{
	const UWorld* World = GEngine->GameViewport->GetWorld();
	const auto EnhancedCamera = Cast<AECRTSCamera>(UGameplayStatics::GetActorOfClass(World, AECRTSCamera::StaticClass()));
	return EnhancedCamera;
}
