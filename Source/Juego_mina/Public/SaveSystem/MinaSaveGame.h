#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MinaSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FPlayerSaveData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FVector Location;

	UPROPERTY(BlueprintReadWrite)
	FRotator Rotation;

	UPROPERTY(BlueprintReadWrite)
	float Health;

	UPROPERTY(BlueprintReadWrite)
	float Stamina;

	UPROPERTY(BlueprintReadWrite)
	TArray<FGameplayTag> GameplayTags;

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, int32> Inventory;
};

USTRUCT(BlueprintType)
struct FPuzzleSaveData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString PuzzleID;

	UPROPERTY(BlueprintReadWrite)
	EPuzzleState State;

	UPROPERTY(BlueprintReadWrite)
	float CompletionTime;

	UPROPERTY(BlueprintReadWrite)
	int32 AttemptCount;
};

UCLASS(Blueprintable, BlueprintType)
class JUEGO_MINA_API UMinaSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UMinaSaveGame();

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FString SaveName;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FDateTime SaveDateTime;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FString LevelName;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FPlayerSaveData PlayerData;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	TArray<FPuzzleSaveData> PuzzleData;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	TArray<FString> CompletedObjectives;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	float PlayTime;
};