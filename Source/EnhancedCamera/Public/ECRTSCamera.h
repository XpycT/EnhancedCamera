// Copyright 2024 XpycT. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ECRTSCamera.generated.h"

struct FInputActionValue;
class UCameraComponent;
class USpringArmComponent;
class USphereComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class ENHANCEDCAMERA_API AECRTSCamera : public APawn
{
	GENERATED_BODY()

public:
	AECRTSCamera();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Camera")
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Camera")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enhanced Camera | Inputs")
	TObjectPtr<UInputMappingContext> InputMappingContext;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enhanced Camera | Inputs")
	TObjectPtr<UInputAction> RotateCameraAxis;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enhanced Camera | Inputs")
	TObjectPtr<UInputAction> MoveCameraAxis;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enhanced Camera | Inputs")
	TObjectPtr<UInputAction> ZoomCamera;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enhanced Camera | Zoom Settings")
	float MinimumZoom;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enhanced Camera | Zoom Settings")
	float MaximumZoom;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enhanced Camera | Zoom Settings")
	float ZoomSpeed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enhanced Camera | Zoom Settings")
	float ZoomSmoothness;


protected:
	virtual void BeginPlay() override;
	
	UPROPERTY()
	APlayerController* PlayerController;

	void OnZoomCamera(const FInputActionValue& Value);
	void OnRotateCamera(const FInputActionValue& Value);
	void OnMoveCamera(const FInputActionValue& Value);

	UPROPERTY()
	float DesiredZoom;

private:
	void BindInputMappingContext() const;
	void ApplyCameraZoomToDesired(float DeltaTime) const;
	
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
