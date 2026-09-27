#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MinaGameInstance.generated.h"

class UMinaSaveGame;

UCLASS(Blueprintable, BlueprintType)
class JUEGO_MINA_API UMinaGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UMinaGameInstance();

	virtual void Init() override;
	virtual void Shutdown() override;

	UFUNCTION(BlueprintCallable, Category = "Game Instance")
	void SaveGame(const FString& SlotName = TEXT("MinaSave"), int32 UserIndex = 0);

	UFUNCTION(BlueprintCallable, Category = "Game Instance")
	void LoadGame(const FString& SlotName = TEXT("MinaSave"), int32 UserIndex = 0);

	UFUNCTION(BlueprintCallable, Category = "Game Instance")
	void NewGame();

	UFUNCTION(BlueprintCallable, Category = "Game Instance")
	void QuitGame();

	UFUNCTION(BlueprintPure, Category = "Game Instance")
	UMinaSaveGame* GetCurrentSaveGame() const { return CurrentSaveGame; }

	UFUNCTION(BlueprintPure, Category = "Game Instance")
	bool HasSaveGame() const { return CurrentSaveGame != nullptr; }

protected:
	UPROPERTY()
	TObjectPtr<UMinaSaveGame> CurrentSaveGame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Instance")
	FString DefaultSaveSlot = TEXT("MinaSave");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Instance")
	int32 DefaultUserIndex = 0;
};