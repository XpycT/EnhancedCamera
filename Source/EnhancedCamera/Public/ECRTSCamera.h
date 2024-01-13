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

USTRUCT()
struct FMoveCameraCommand
{
	GENERATED_BODY()
	UPROPERTY()
	float X = 0;
	UPROPERTY()
	float Y = 0;
	UPROPERTY()
	float Scale = 0;
};

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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enhanced Camera | Inputs")
	TObjectPtr<UInputAction> ResetCamera;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Camera | Movement Settings")
	float MoveSpeed = 50.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enhanced Camera | Zoom Settings")
	float MinimumZoom;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enhanced Camera | Zoom Settings")
	float MaximumZoom;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enhanced Camera | Zoom Settings")
	float ZoomSpeed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enhanced Camera | Zoom Settings")
	float ZoomSmoothness;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enhanced Camera | Rotate Settings")
	float ViewPitchMax = 60.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enhanced Camera | Rotate Settings")
	float ViewPitchMin = -20.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enhanced Camera | Rotate Settings")
	float RotateSpeed = 50.0f;


protected:
	void SetInitialValues();
	virtual void BeginPlay() override;
	
	UPROPERTY()
	APlayerController* PlayerController;

	UFUNCTION()
	void OnZoomCamera(const FInputActionValue& Value);
	UFUNCTION()
	void OnRotateCamera(const FInputActionValue& Value);
	UFUNCTION()
	void OnMoveCamera(const FInputActionValue& Value);
	UFUNCTION()
	void OnResetCamera(const FInputActionValue& Value);
	

	void ApplyMoveCameraCommands();

	float DeltaSeconds;
	UPROPERTY()
	float DesiredZoom;

private:
	void BindInputMappingContext() const;
	void ApplyCameraZoomToDesired() const;
	void RequestMoveCamera(const float X, const float Y, const float Scale);
	UPROPERTY()
	TArray<FMoveCameraCommand> MoveCameraCommands;

	float DefaultZoom;
	FRotator DefaultRotation;
	
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
