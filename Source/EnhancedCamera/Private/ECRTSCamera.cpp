// Copyright 2024 XpycT. All Rights Reserved.


#include "ECRTSCamera.h"

#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/WidgetLayoutLibrary.h"
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

	bUseControllerRotationPitch = true;
	bUseControllerRotationRoll = true;
	bUseControllerRotationYaw = true;

	MaximumZoom = 5000;
	MinimumZoom = 500;
	ZoomSmoothness = 3;
	ZoomSpeed = 200;

	EnableDynamicCameraHeight = true;
	FindGroundTraceLength = 100000.f;
	CollisionChannel = ECC_WorldStatic;
}

void AECRTSCamera::SetInitialValues()
{
	PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = true;
	PlayerController->PlayerCameraManager->ViewPitchMax = ViewPitchMax;
	PlayerController->PlayerCameraManager->ViewPitchMin = ViewPitchMin;
	
	DefaultZoom = SpringArm->TargetArmLength;
	DesiredZoom = DefaultZoom;
	DefaultRotation = GetControlRotation();

	if(EnableEdgeScrolling)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		InputMode.SetHideCursorDuringCapture(false);
		PlayerController->SetInputMode(InputMode);
	}
}

// Called when the game starts or when spawned
void AECRTSCamera::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle Handler;
	GetWorldTimerManager().SetTimer(Handler, this, &AECRTSCamera::SetBoundaries, 0.1f, false, 0.2f);
	GEngine->GameViewport->Viewport->ViewportResizedEvent.AddUObject(this, &AECRTSCamera::ViewportSizeChanged);

	SetInitialValues();	
	BindInputMappingContext();
}


void AECRTSCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DeltaSeconds = DeltaTime;
	ApplyCameraZoomToDesired();
	ApplyMoveCameraCommands();
	ApplyEdgeScrolling();
	ApplyDynamicCameraHeight();
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
	if (Controller != nullptr)
	{
		const FVector2D LookValue = Value.Get<FVector2D>();
 
		if (LookValue.X != 0.f)
		{
			AddControllerYawInput(LookValue.X * RotateSpeed * DeltaSeconds);
		}
 
		if (LookValue.Y != 0.f)
		{
			AddControllerPitchInput(LookValue.Y * RotateSpeed * DeltaSeconds);
		}
	}
}

void AECRTSCamera::OnMoveCamera(const FInputActionValue& Value)
{
	if(CameraFollowTarget != nullptr)
	{
		UnFollowTarget();
	}
	
	RequestMoveCamera(
		SpringArm->GetRightVector().X,
		SpringArm->GetRightVector().Y,
		Value.Get<FVector2D>().X
	);

	RequestMoveCamera(
		SpringArm->GetForwardVector().X,
		SpringArm->GetForwardVector().Y,
		Value.Get<FVector2D>().Y
	);
}

void AECRTSCamera::OnResetCamera(const FInputActionValue& Value)
{
	DesiredZoom = DefaultZoom;
	Controller->SetControlRotation(DefaultRotation);
}

void AECRTSCamera::ApplyMoveCameraCommands()
{
	for (const auto& [X, Y, Scale] : MoveCameraCommands)
	{
		auto Movement = FVector2D(X, Y);
		Movement.Normalize();
		Movement *= MoveSpeed * Scale * DeltaSeconds;
		SetActorLocation(GetActorLocation() + FVector(Movement.X, Movement.Y, 0.0f));
	}

	MoveCameraCommands.Empty();
}

void AECRTSCamera::RequestMoveCamera(const float X, const float Y, const float Scale)
{
	FMoveCameraCommand MoveCameraCommand;
	MoveCameraCommand.X = X;
	MoveCameraCommand.Y = Y;
	MoveCameraCommand.Scale = Scale;
	MoveCameraCommands.Push(MoveCameraCommand);
}

bool AECRTSCamera::IsValidMousePosition(const FVector2D Positions, const FVector4& Rules)
{
	const bool InputY = Positions.Y >= Rules.X && Positions.Y <= Rules.Y;
	const bool InputX = Positions.X >= Rules.Z && Positions.X <= Rules.W;
	return InputX && InputY;

}

void AECRTSCamera::BindInputMappingContext() const
{
	if (PlayerController && PlayerController->GetLocalPlayer())
	{
		if (const auto Input = PlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (!Input->HasMappingContext(InputMappingContext))
			{
				Input->ClearAllMappings();
				Input->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}

void AECRTSCamera::ApplyCameraZoomToDesired() const
{
	SpringArm->TargetArmLength = FMath::FInterpTo(
		SpringArm->TargetArmLength,
		DesiredZoom,
		DeltaSeconds,
		ZoomSmoothness
	);
}

void AECRTSCamera::ApplyDynamicCameraHeight()
{
	if (EnableDynamicCameraHeight)
	{
		const auto RootWorldLocation = GetActorLocation();
		const TArray<AActor*> ActorsToIgnore;

		auto HitResult = FHitResult();
		auto DidHit = UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			FVector(RootWorldLocation.X, RootWorldLocation.Y, RootWorldLocation.Z + FindGroundTraceLength),
			FVector(RootWorldLocation.X, RootWorldLocation.Y, RootWorldLocation.Z - FindGroundTraceLength),
			UEngineTypes::ConvertToTraceType(CollisionChannel),
			true,
			ActorsToIgnore,
			EDrawDebugTrace::Type::None,
			HitResult,
			true
		);

		if (DidHit)
		{
			SetActorLocation(FVector( HitResult.Location.X, HitResult.Location.Y, HitResult.Location.Z ));
		}
	}
}

void AECRTSCamera::ApplyEdgeScrolling()
{
	if (!EnableEdgeScrolling)
	{
		return;
	}

	float DirectionX = 0;
	float DirectionY = 0;

	FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	MousePosition = MousePosition * UWidgetLayoutLibrary::GetViewportScale(GetWorld());
	if (this->IsValidMousePosition(MousePosition, Top))
	{
		DirectionX = 1;
	}
	else if (IsValidMousePosition(MousePosition, Bottom))
	{
		DirectionX = -1;
	}

	if (IsValidMousePosition(FVector2D(MousePosition.Y, MousePosition.X), Right))
	{
		DirectionY = 1;
	}
	else if (IsValidMousePosition(FVector2D(MousePosition.Y, MousePosition.X), Left))
	{
		DirectionY = -1;
	}

	RequestMoveCamera(
		SpringArm->GetRightVector().X,
		SpringArm->GetRightVector().Y,
		DirectionY
	);

	RequestMoveCamera(
		SpringArm->GetForwardVector().X,
		SpringArm->GetForwardVector().Y,
		DirectionX
	);
}

void AECRTSCamera::SetBoundaries()
{
	const FVector2D Result = UWidgetLayoutLibrary::GetViewportSize(GetWorld());

	Top = FVector4(0, 0, 0, 0);
	Bottom = FVector4(0, 0, 0, 0);
	Left = FVector4(0, 0, 0, 0);
	Right = FVector4(0, 0, 0, 0);

	Top.Y = (MovementZoneInPercent / 100.f) * Result.Y;
	Top.W = Result.X;

	Bottom.X = (1 - (MovementZoneInPercent / 100.f)) * Result.Y;
	Bottom.Y = Result.Y;
	Bottom.W = Result.X;

	Left.Y = (MovementZoneInPercent / 100.f) * Result.X;
	Left.W = Result.Y;

	Right.X = (1 - (MovementZoneInPercent / 100.f)) * Result.X;
	Right.Y = Result.X;
	Right.W = Result.Y;

	if (!EnableEdgeScrolling)
	{
		Top.X = -Deactivate;
		Top.Z = -Deactivate;
		Top.W = Deactivate;

		Bottom.Y = Deactivate;
		Bottom.Z = -Deactivate;
		Bottom.W = Deactivate;

		Left.X = -Deactivate;
		Left.Z = -Deactivate;
		Left.W = Deactivate;

		Right.Y = Deactivate;
		Right.Z = -Deactivate;
		Right.W = Deactivate;
	}

}

void AECRTSCamera::ViewportSizeChanged(FViewport* ViewPort, uint32 val)
{
	SetBoundaries();
}

void AECRTSCamera::FollowTargetIfSet()
{
	if (CameraFollowTarget != nullptr)
	{
		SetActorLocation(CameraFollowTarget->GetActorLocation());
	}
}

void AECRTSCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if(const auto EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(RotateCameraAxis, ETriggerEvent::Triggered, this, &AECRTSCamera::OnRotateCamera);
		EnhancedInputComponent->BindAction(MoveCameraAxis, ETriggerEvent::Triggered, this, &AECRTSCamera::OnMoveCamera);
		EnhancedInputComponent->BindAction(ZoomCamera, ETriggerEvent::Triggered, this, &AECRTSCamera::OnZoomCamera);
		EnhancedInputComponent->BindAction(ResetCamera, ETriggerEvent::Triggered, this, &AECRTSCamera::OnResetCamera);
	}
}

void AECRTSCamera::FollowTarget(AActor* Target)
{
	CameraFollowTarget = Target;
}

void AECRTSCamera::UnFollowTarget()
{
	CameraFollowTarget = nullptr;
}
