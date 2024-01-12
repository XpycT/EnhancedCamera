// Copyright 2024 XpycT. All Rights Reserved.


#include "ECRTSCamera.h"

#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"

AECRTSCamera::AECRTSCamera()
{
 	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SetRootComponent(SphereComponent);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeRotation(FRotator(-65, 0, 0));
	SpringArm->TargetArmLength = 4000;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 5;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraRotationLagSpeed = 5;
	SpringArm->bDoCollisionTest = false;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera");
	CameraComponent->SetupAttachment(SpringArm);

	MaximumZoom = 5000;
	MinimumZoom = 500;
	ZoomSmoothness = 3;
	ZoomSpeed = 200;
}

// Called when the game starts or when spawned
void AECRTSCamera::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = GetWorld()->GetFirstPlayerController();
	DesiredZoom = SpringArm->TargetArmLength;
	
	BindInputMappingContext();
}


void AECRTSCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ApplyCameraZoomToDesired(DeltaTime);
}

void AECRTSCamera::OnZoomCamera(const FInputActionValue& Value)
{
	DesiredZoom = FMath::Clamp(
		DesiredZoom + Value.GetMagnitude() * -ZoomSpeed,
		MinimumZoom,
		MaximumZoom
	);
}

void AECRTSCamera::OnRotateCamera(const FInputActionValue& Value)
{
}

void AECRTSCamera::OnMoveCamera(const FInputActionValue& Value)
{
}

void AECRTSCamera::BindInputMappingContext() const
{
	if (PlayerController && PlayerController->GetLocalPlayer())
	{
		if (const auto Input = PlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			PlayerController->bShowMouseCursor = true;

			if (!Input->HasMappingContext(InputMappingContext))
			{
				Input->ClearAllMappings();
				Input->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}

void AECRTSCamera::ApplyCameraZoomToDesired(float DeltaTime) const
{
	SpringArm->TargetArmLength = FMath::FInterpTo(
		SpringArm->TargetArmLength,
		DesiredZoom,
		DeltaTime,
		ZoomSmoothness
	);
}

void AECRTSCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if(const auto EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(RotateCameraAxis, ETriggerEvent::Triggered, this, &AECRTSCamera::OnRotateCamera);
		EnhancedInputComponent->BindAction(MoveCameraAxis, ETriggerEvent::Triggered, this, &AECRTSCamera::OnMoveCamera);
		EnhancedInputComponent->BindAction(ZoomCamera, ETriggerEvent::Triggered, this, &AECRTSCamera::OnZoomCamera);
	}
}

