#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MinaGameMode.generated.h"

class AMinaCharacter;
class AMinaPlayerController;
class APuzzleManager;

UCLASS(Blueprintable, BlueprintType)
class JUEGO_MINA_API AMinaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMinaGameMode();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "GameMode")
	static AMinaGameMode* GetMinaGameMode(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "GameMode")
	virtual void OnPlayerDeath(AMinaCharacter* PlayerCharacter);

	UFUNCTION(BlueprintCallable, Category = "GameMode")
	virtual void OnPlayerWin();

	UFUNCTION(BlueprintCallable, Category = "GameMode")
	void RestartGame();

	UFUNCTION(BlueprintCallable, Category = "GameMode")
	void QuitGame();

	UFUNCTION(BlueprintPure, Category = "GameMode")
	APuzzleManager* GetPuzzleManager() const { return PuzzleManager; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GameMode")
	TSubclassOf<AMinaCharacter> DefaultCharacterClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GameMode")
	TSubclassOf<AMinaPlayerController> DefaultPlayerControllerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GameMode")
	TSubclassOf<APuzzleManager> PuzzleManagerClass;

	UPROPERTY()
	TObjectPtr<APuzzleManager> PuzzleManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode")
	float RespawnDelay = 3.0f;
};