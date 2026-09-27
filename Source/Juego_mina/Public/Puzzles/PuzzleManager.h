#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Puzzles/PuzzleBase.h"
#include "PuzzleManager.generated.h"

class APuzzleBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPuzzleCompleted, APuzzleBase*, CompletedPuzzle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPuzzleFailed, APuzzleBase*, FailedPuzzle);

USTRUCT(BlueprintType)
struct FPuzzleProgress
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
class JUEGO_MINA_API APuzzleManager : public AActor
{
	GENERATED_BODY()

public:
	APuzzleManager();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Puzzle Manager")
	static APuzzleManager* GetPuzzleManager(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Puzzle Manager")
	void RegisterPuzzle(APuzzleBase* Puzzle, const FString& PuzzleID);

	UFUNCTION(BlueprintCallable, Category = "Puzzle Manager")
	void UnregisterPuzzle(APuzzleBase* Puzzle);

	UFUNCTION(BlueprintCallable, Category = "Puzzle Manager")
	void OnPuzzleStateChanged(APuzzleBase* Puzzle, EPuzzleState NewState);

	UFUNCTION(BlueprintCallable, Category = "Puzzle Manager")
	EPuzzleState GetPuzzleState(const FString& PuzzleID) const;

	UFUNCTION(BlueprintCallable, Category = "Puzzle Manager")
	bool IsPuzzleSolved(const FString& PuzzleID) const;

	UFUNCTION(BlueprintCallable, Category = "Puzzle Manager")
	TArray<FString> GetSolvedPuzzles() const;

	UFUNCTION(BlueprintCallable, Category = "Puzzle Manager")
	TArray<FString> GetActivePuzzles() const;

	UFUNCTION(BlueprintCallable, Category = "Puzzle Manager")
	void ResetAllPuzzles();

	UFUNCTION(BlueprintCallable, Category = "Puzzle Manager")
	void SavePuzzleProgress();

	UFUNCTION(BlueprintCallable, Category = "Puzzle Manager")
	void LoadPuzzleProgress();

	UPROPERTY(BlueprintAssignable, Category = "Puzzle Manager Events")
	FPuzzleCompleted OnPuzzleCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Puzzle Manager Events")
	FPuzzleFailed OnPuzzleFailed;

protected:
	UPROPERTY()
	TMap<FString, TObjectPtr<APuzzleBase>> RegisteredPuzzles;

	UPROPERTY()
	TMap<FString, FPuzzleProgress> PuzzleProgress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle Manager")
	FString SaveSlotName = TEXT("PuzzleProgress");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle Manager")
	int32 SaveUserIndex = 0;
};