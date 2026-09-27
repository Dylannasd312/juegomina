#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MinaSaveManager.generated.h"

class UMinaSaveGame;
class AMinaCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameSaved, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameLoaded, bool, bSuccess);

UCLASS(Blueprintable, BlueprintType)
class JUEGO_MINA_API AMinaSaveManager : public AActor
{
	GENERATED_BODY()

public:
	AMinaSaveManager();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Save Manager")
	static AMinaSaveManager* GetSaveManager(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Save Manager")
	void SaveGame(const FString& SlotName, int32 UserIndex = 0);

	UFUNCTION(BlueprintCallable, Category = "Save Manager")
	void LoadGame(const FString& SlotName, int32 UserIndex = 0);

	UFUNCTION(BlueprintCallable, Category = "Save Manager")
	void DeleteSave(const FString& SlotName, int32 UserIndex = 0);

	UFUNCTION(BlueprintCallable, Category = "Save Manager")
	bool DoesSaveExist(const FString& SlotName, int32 UserIndex = 0) const;

	UFUNCTION(BlueprintCallable, Category = "Save Manager")
	TArray<FString> GetSaveSlots() const;

	UFUNCTION(BlueprintCallable, Category = "Save Manager")
	void AutoSave();

	UFUNCTION(BlueprintPure, Category = "Save Manager")
	UMinaSaveGame* GetCurrentSaveGame() const { return CurrentSaveGame; }

	UPROPERTY(BlueprintAssignable, Category = "Save Manager Events")
	FOnGameSaved OnGameSaved;

	UPROPERTY(BlueprintAssignable, Category = "Save Manager Events")
	FOnGameLoaded OnGameLoaded;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Manager")
	FString DefaultSlotName = TEXT("MinaSave");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Manager")
	int32 DefaultUserIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Manager")
	float AutoSaveInterval = 300.0f;

	UPROPERTY()
	TObjectPtr<UMinaSaveGame> CurrentSaveGame;

	FTimerHandle AutoSaveTimer;

	void CapturePlayerData(AMinaCharacter* Character);
	void ApplyPlayerData(AMinaCharacter* Character);
	void CapturePuzzleData();
	void ApplyPuzzleData();
};