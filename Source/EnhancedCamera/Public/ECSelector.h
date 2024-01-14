// Copyright 2024 XpycT. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ECSelector.generated.h"

struct FInputActionValue;
class UECSelectable;
class AECHUD;
class UInputAction;
class UInputMappingContext;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorsSelected, const TArray<AActor*>&, SelectedActors);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ENHANCEDCAMERA_API UECSelector : public UActorComponent
{
	GENERATED_BODY()

public:	
	UECSelector();

	UPROPERTY(BlueprintAssignable)
	FOnActorsSelected OnActorsSelected;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enhanced Camera | Inputs")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enhanced Camera | Inputs")
	UInputAction* BeginSelection;

	// Function to clear selected actors, can be overridden in Blueprints
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Enhanced Camera | Selection")
	void ClearSelectedActors();

	// Function to handle selected actors, can be overridden in Blueprints
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Enhanced Camera | Selection")
	void HandleSelectedActors(const TArray<AActor*>& NewSelectedActors);

	// BlueprintCallable to allow calling from Blueprints
	UFUNCTION(BlueprintCallable, Category = "Enhanced Camera | Selection")
	void OnSelectionStart(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Enhanced Camera | Selection")
	void OnUpdateSelection(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Enhanced Camera | Selection")
	void OnSelectionEnd(const FInputActionValue& Value);

	UPROPERTY(BlueprintReadOnly, Category = "Enhanced Camera | Selection")
	TArray<UECSelectable*> SelectedActors;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	AECHUD* HUD;

	FVector2D SelectionStart;
	FVector2D SelectionEnd;

	bool bIsSelecting;

	void BindInputActions();
	void BindInputMappingContext() const;
	void CollectComponentDependencyReferences();	
};
