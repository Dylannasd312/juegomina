#include "Gameplay/MinaGameMode.h"
#include "Gameplay/MinaPlayerController.h"
#include "Character/MinaCharacter.h"
#include "Puzzles/PuzzleManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AMinaGameMode::AMinaGameMode()
{
	DefaultPawnClass = AMinaCharacter::StaticClass();
	PlayerControllerClass = AMinaPlayerController::StaticClass();
}

void AMinaGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (PuzzleManagerClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		PuzzleManager = GetWorld()->SpawnActor<APuzzleManager>(PuzzleManagerClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	}
}

AMinaGameMode* AMinaGameMode::GetMinaGameMode(UObject* WorldContextObject)
{
	if (UWorld* World = WorldContextObject->GetWorld())
	{
		return Cast<AMinaGameMode>(World->GetAuthGameMode());
	}
	return nullptr;
}

void AMinaGameMode::OnPlayerDeath(AMinaCharacter* PlayerCharacter)
{
	if (!PlayerCharacter)
	{
		return;
	}

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [this]()
	{
		RestartGame();
	}, RespawnDelay, false);
}

void AMinaGameMode::OnPlayerWin()
{
}

void AMinaGameMode::RestartGame()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void AMinaGameMode::QuitGame()
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
	}
}