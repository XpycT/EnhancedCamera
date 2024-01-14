// Copyright 2024 XpycT. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ECSelectable.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ENHANCEDCAMERA_API UECSelectable : public UActorComponent
{
	GENERATED_BODY()

public:	
	UECSelectable();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, category = "Enhanced Camera | Selection")
	void OnSelected();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, category = "Enhanced Camera | Selection")
	void OnDeselected();
};
