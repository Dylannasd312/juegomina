#include "GameInstance/MinaGameInstance.h"
#include "SaveSystem/MinaSaveGame.h"
#include "Kismet/GameplayStatics.h"

UMinaGameInstance::UMinaGameInstance()
{
}

void UMinaGameInstance::Init()
{
	Super::Init();
}

void UMinaGameInstance::Shutdown()
{
	Super::Shutdown();
}

void UMinaGameInstance::SaveGame(const FString& SlotName, int32 UserIndex)
{
	CurrentSaveGame = Cast<UMinaSaveGame>(UGameplayStatics::CreateSaveGameObject(UMinaSaveGame::StaticClass()));
	if (!CurrentSaveGame)
	{
		return;
	}

	CurrentSaveGame->SaveName = SlotName;
	CurrentSaveGame->SaveDateTime = FDateTime::Now();

	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, UserIndex);
}

void UMinaGameInstance::LoadGame(const FString& SlotName, int32 UserIndex)
{
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		return;
	}

	CurrentSaveGame = Cast<UMinaSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
}

void UMinaGameInstance::NewGame()
{
	CurrentSaveGame = Cast<UMinaSaveGame>(UGameplayStatics::CreateSaveGameObject(UMinaSaveGame::StaticClass()));
	if (CurrentSaveGame)
	{
		CurrentSaveGame->SaveName = DefaultSaveSlot;
		CurrentSaveGame->SaveDateTime = FDateTime::Now();
		CurrentSaveGame->LevelName = TEXT("MainMenu");
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName("MainMenu"));
}

void UMinaGameInstance::QuitGame()
{
	if (APlayerController* PC = GetFirstLocalPlayerController())
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), PC, EQuitPreference::Quit, false);
	}
}