// Copyright 2024 XpycT. All Rights Reserved.


#include "ECSelector.h"

#include "ECHUD.h"
#include "ECSelectable.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

UECSelector::UECSelector()
:
	InputMappingContext(nullptr), BeginSelection(nullptr),
	PlayerController(nullptr), HUD(nullptr), bIsSelecting(false)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UECSelector::BeginPlay()
{
	Super::BeginPlay();
	const auto NetMode = GetNetMode();
	if(NetMode != NM_DedicatedServer)
	{
		CollectComponentDependencyReferences();
		BindInputMappingContext();
		BindInputActions();
		OnActorsSelected.AddDynamic(this, &UECSelector::HandleSelectedActors);
	}
}

void UECSelector::HandleSelectedActors_Implementation(const TArray<AActor*>& NewSelectedActors)
{
	// Convert NewSelectedActors to a set for efficient lookup
	TSet<AActor*> NewSelectedActorSet;
	for (const auto& Actor : NewSelectedActors)
	{
		NewSelectedActorSet.Add(Actor);
	}

	// Iterate over currently selected actors
	for (const auto& Selected : SelectedActors)
	{
		// Check if the actor is not in the new selection
		if (!NewSelectedActorSet.Contains(Selected->GetOwner()))
		{
			// Call OnDeselected for actors that are no longer selected
			Selected->OnDeselected();
		}
	}

	// Clear the current selection
	ClearSelectedActors();
    
	// Add new selected actors and call OnSelected
	for (const auto& Actor : NewSelectedActors)
	{
		if (UECSelectable* SelectableComponent = Actor->FindComponentByClass<UECSelectable>())
		{
			SelectedActors.Add(SelectableComponent);
			SelectableComponent->OnSelected();
		}
	}
}

void UECSelector::ClearSelectedActors_Implementation()
{
	SelectedActors.Empty();
}

void UECSelector::OnSelectionStart(const FInputActionValue& Value)
{
	FVector2D MousePosition;
	PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);
	HUD->BeginSelection(MousePosition);
}

void UECSelector::OnUpdateSelection(const FInputActionValue& Value)
{
	FVector2D MousePosition;
	PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);
	SelectionEnd = MousePosition;
	HUD->UpdateSelection(SelectionEnd);
}

void UECSelector::OnSelectionEnd(const FInputActionValue& Value)
{
	// Call PerformSelection on the HUD to execute selection logic
	HUD->EndSelection();
}

void UECSelector::BindInputActions()
{
	if (const auto EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
	{
		EnhancedInputComponent->BindAction(
			BeginSelection,
			ETriggerEvent::Started,
			this,
			&UECSelector::OnSelectionStart
		);

		EnhancedInputComponent->BindAction(
			BeginSelection,
			ETriggerEvent::Triggered,
			this,
			&UECSelector::OnUpdateSelection
		);

		EnhancedInputComponent->BindAction(
			BeginSelection,
			ETriggerEvent::Completed,
			this,
			&UECSelector::OnSelectionEnd
		);
	}
}

void UECSelector::BindInputMappingContext() const
{
	if (PlayerController && PlayerController->GetLocalPlayer())
	{
		if (const auto Input = PlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			PlayerController->bShowMouseCursor = true;

			// Check if the context is already bound to prevent double binding
			if (!Input->HasMappingContext(this->InputMappingContext))
			{
				Input->ClearAllMappings();
				Input->AddMappingContext(this->InputMappingContext, 0);
			}
		}
	}
}

void UECSelector::CollectComponentDependencyReferences()
{
	if (const auto PlayerControllerRef = GetWorld()->GetFirstPlayerController())
	{
		PlayerController = PlayerControllerRef;
		HUD = Cast<AECHUD>(PlayerControllerRef->GetHUD());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("USelector is not attached to a PlayerController."));
	}
}

