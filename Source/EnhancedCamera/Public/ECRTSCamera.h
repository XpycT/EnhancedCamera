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
	float MoveSpeed = 5000.0f;	
	// Ignores the boundaries, allows movement with the mouse off the viewport
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Camera | Movement Settings")
	bool EnableEdgeScrolling = true;
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Enhanced Camera | Movement Settings",
		meta=(EditCondition="EnableEdgeScrolling")
		)
	int MovementZoneInPercent = 5;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enhanced Camera | Height Adjustment")
	bool EnableDynamicCameraHeight;
	UPROPERTY(
		BlueprintReadWrite,
		EditAnywhere,
		Category = "Enhanced Camera | Height Adjustment",
		meta=(EditCondition="EnableDynamicCameraHeight")
		)
	TEnumAsByte<ECollisionChannel> CollisionChannel;
	UPROPERTY(
		BlueprintReadWrite,
		EditAnywhere,
		Category = "Enhanced Camera | Height Adjustment",
		meta=(EditCondition="EnableDynamicCameraHeight")
		)
	float FindGroundTraceLength;
	
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
	void ApplyCameraZoomToDesired() const;
	void ApplyDynamicCameraHeight();
	void ApplyEdgeScrolling();

	UPROPERTY()
	float DeltaSeconds;
	UPROPERTY()
	float DesiredZoom;

private:
	void BindInputMappingContext() const;
	void RequestMoveCamera(const float X, const float Y, const float Scale);

	/**
	 * Positions are the positions to check
	 * The Value X(Positions) needs to be between X-Y(Rules) and Y(Positions) between Z-W(Rules) of the defined rules
	 * @link SetBoundaries @endlink 
	 * The Rules define the bounding Box
	 */
	static bool IsValidMousePosition(const FVector2D Positions, const FVector4& Rules);
	void SetBoundaries();
	void ViewportSizeChanged(FViewport* ViewPort, uint32 val);

	void FollowTargetIfSet();
	
	UPROPERTY()
	TArray<FMoveCameraCommand> MoveCameraCommands;
	UPROPERTY()
	AActor* CameraFollowTarget;

	float DefaultZoom;
	FRotator DefaultRotation;
	
	const int Deactivate = 999999;
	// X ist the minimal value and Y the maximum value
	FVector4 Top = FVector4();
	FVector4 Bottom = FVector4();
	FVector4 Left = FVector4();
	FVector4 Right = FVector4();
	
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Enhanced Camera")
	void FollowTarget(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Enhanced Camera")
	void UnFollowTarget();
};
