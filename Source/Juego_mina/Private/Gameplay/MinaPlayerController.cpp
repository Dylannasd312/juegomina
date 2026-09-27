#include "Gameplay/MinaPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "GameplayStatics.h"
#include "Kismet/GameplayStatics.h"
#include "Character/MinaCharacter.h"

AMinaPlayerController::AMinaPlayerController()
{
	bShowMouseCursor = false;
	bEnableClickEvents = false;
	bEnableMouseOverEvents = false;
}

void AMinaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (GameplayMappingContext)
	{
		SetInputMappingContext(GameplayMappingContext, 0);
	}

	SetInputMode(FInputModeGameOnly());
}

void AMinaPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (PauseAction)
		{
			EnhancedInput->BindAction(PauseAction, ETriggerEvent::Triggered, this, &AMinaPlayerController::OnPause);
		}
		if (InteractAction)
		{
			EnhancedInput->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AMinaPlayerController::OnInteract);
		}
		if (InventoryAction)
		{
			EnhancedInput->BindAction(InventoryAction, ETriggerEvent::Triggered, this, &AMinaPlayerController::OnInventory);
		}
	}
}

void AMinaPlayerController::SetInputMappingContext(TObjectPtr<UInputMappingContext> NewContext, int32 Priority)
{
	if (NewContext && ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(NewContext, Priority);
		}
	}
}

void AMinaPlayerController::RemoveInputMappingContext(TObjectPtr<UInputMappingContext> Context)
{
	if (Context && ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->RemoveMappingContext(Context);
		}
	}
}

void AMinaPlayerController::TogglePause()
{
	bIsPaused = !bIsPaused;

	if (bIsPaused)
	{
		SetPause(true);
		SetInputMode(FInputModeGameAndUI());
		bShowMouseCursor = true;
		if (UIMappingContext)
		{
			SetInputMappingContext(UIMappingContext, 10);
		}
	}
	else
	{
		SetPause(false);
		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
		if (UIMappingContext)
		{
			RemoveInputMappingContext(UIMappingContext);
		}
	}
}

void AMinaPlayerController::RestartLevel()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void AMinaPlayerController::OnPause(const FInputActionValue& Value)
{
	TogglePause();
}

void AMinaPlayerController::OnInteract(const FInputActionValue& Value)
{
	if (AMinaCharacter* Character = Cast<AMinaCharacter>(GetPawn()))
	{
		Character->Interact();
	}
}

void AMinaPlayerController::OnInventory(const FInputActionValue& Value)
{
}