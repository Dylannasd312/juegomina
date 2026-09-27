#include "SaveSystem/MinaSaveManager.h"
#include "SaveSystem/MinaSaveGame.h"
#include "Character/MinaCharacter.h"
#include "Puzzles/PuzzleManager.h"
#include "Puzzles/PuzzleBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

AMinaSaveManager::AMinaSaveManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMinaSaveManager::BeginPlay()
{
	Super::BeginPlay();

	if (AutoSaveInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(AutoSaveTimer, this, &AMinaSaveManager::AutoSave, AutoSaveInterval, true);
	}
}

AMinaSaveManager* AMinaSaveManager::GetSaveManager(UObject* WorldContextObject)
{
	if (UWorld* World = WorldContextObject->GetWorld())
	{
		for (TActorIterator<AMinaSaveManager> It(World); It; ++It)
		{
			return *It;
		}
	}
	return nullptr;
}

void AMinaSaveManager::SaveGame(const FString& SlotName, int32 UserIndex)
{
	CurrentSaveGame = Cast<UMinaSaveGame>(UGameplayStatics::CreateSaveGameObject(UMinaSaveGame::StaticClass()));
	if (!CurrentSaveGame)
	{
		OnGameSaved.Broadcast(false);
		return;
	}

	CurrentSaveGame->SaveName = SlotName;
	CurrentSaveGame->SaveDateTime = FDateTime::Now();
	CurrentSaveGame->LevelName = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);

	if (AMinaCharacter* Character = Cast<AMinaCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		CapturePlayerData(Character);
	}

	CapturePuzzleData();

	bool bSuccess = UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, UserIndex);
	OnGameSaved.Broadcast(bSuccess);
}

void AMinaSaveManager::LoadGame(const FString& SlotName, int32 UserIndex)
{
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		OnGameLoaded.Broadcast(false);
		return;
	}

	CurrentSaveGame = Cast<UMinaSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
	if (!CurrentSaveGame)
	{
		OnGameLoaded.Broadcast(false);
		return;
	}

	if (AMinaCharacter* Character = Cast<AMinaCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		ApplyPlayerData(Character);
	}

	ApplyPuzzleData();

	OnGameLoaded.Broadcast(true);
}

void AMinaSaveManager::DeleteSave(const FString& SlotName, int32 UserIndex)
{
	UGameplayStatics::DeleteGameInSlot(SlotName, UserIndex);
}

bool AMinaSaveManager::DoesSaveExist(const FString& SlotName, int32 UserIndex) const
{
	return UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex);
}

TArray<FString> AMinaSaveManager::GetSaveSlots() const
{
	return UGameplayStatics::GetSaveGameNames();
}

void AMinaSaveManager::AutoSave()
{
	SaveGame(DefaultSlotName, DefaultUserIndex);
}

void AMinaSaveManager::CapturePlayerData(AMinaCharacter* Character)
{
	if (!Character || !CurrentSaveGame)
	{
		return;
	}

	CurrentSaveGame->PlayerData.Location = Character->GetActorLocation();
	CurrentSaveGame->PlayerData.Rotation = Character->GetActorRotation();
	CurrentSaveGame->PlayerData.Health = Character->CurrentHealth;
	CurrentSaveGame->PlayerData.Stamina = Character->CurrentStamina;

	TArray<FGameplayTag> Tags;
	Character->GetOwnedGameplayTags(Tags);
	CurrentSaveGame->PlayerData.GameplayTags = Tags;
}

void AMinaSaveManager::ApplyPlayerData(AMinaCharacter* Character)
{
	if (!Character || !CurrentSaveGame)
	{
		return;
	}

	Character->SetActorLocationAndRotation(CurrentSaveGame->PlayerData.Location, CurrentSaveGame->PlayerData.Rotation);
	Character->CurrentHealth = CurrentSaveGame->PlayerData.Health;
	Character->CurrentStamina = CurrentSaveGame->PlayerData.Stamina;

	for (const FGameplayTag& Tag : CurrentSaveGame->PlayerData.GameplayTags)
	{
		Character->AddGameplayTag(Tag);
	}
}

void AMinaSaveManager::CapturePuzzleData()
{
	if (!CurrentSaveGame)
	{
		return;
	}

	CurrentSaveGame->PuzzleData.Empty();

	if (APuzzleManager* PuzzleManager = APuzzleManager::GetPuzzleManager(this))
	{
		for (const auto& Pair : PuzzleManager->GetRegisteredPuzzles())
		{
			FPuzzleSaveData SaveData;
			SaveData.PuzzleID = Pair.Key;
			SaveData.State = Pair.Value->GetPuzzleState();
			SaveData.CompletionTime = 0.0f;
			SaveData.AttemptCount = 0;

			CurrentSaveGame->PuzzleData.Add(SaveData);
		}
	}
}

void AMinaSaveManager::ApplyPuzzleData()
{
	if (!CurrentSaveGame)
	{
		return;
	}

	if (APuzzleManager* PuzzleManager = APuzzleManager::GetPuzzleManager(this))
	{
		for (const FPuzzleSaveData& SaveData : CurrentSaveGame->PuzzleData)
		{
			APuzzleBase* Puzzle = PuzzleManager->GetRegisteredPuzzle(SaveData.PuzzleID);
			if (Puzzle)
			{
				switch (SaveData.State)
				{
				case EPuzzleState::Solved:
					Puzzle->SolvePuzzle();
					break;
				case EPuzzleState::Active:
					Puzzle->ActivatePuzzle();
					break;
				case EPuzzleState::Failed:
					Puzzle->FailPuzzle();
					break;
				default:
					Puzzle->ResetPuzzle();
					break;
				}
			}
		}
	}
}