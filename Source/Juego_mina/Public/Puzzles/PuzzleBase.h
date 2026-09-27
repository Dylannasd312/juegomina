#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "MinaInteractableInterface.h"
#include "PuzzleBase.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class AMinaCharacter;

UENUM(BlueprintType)
enum class EPuzzleState : uint8
{
	Inactive,
	Active,
	Solved,
	Failed
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPuzzleStateChanged, EPuzzleState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPuzzleSolved);

UCLASS(Blueprintable, BlueprintType)
class JUEGO_MINA_API APuzzleBase : public AActor
{
	GENERATED_BODY()

public:
	APuzzleBase();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	virtual void Interact(AMinaCharacter* Interactor);

	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	virtual void ActivatePuzzle();

	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	virtual void DeactivatePuzzle();

	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	virtual void SolvePuzzle();

	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	virtual void FailPuzzle();

	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	virtual void ResetPuzzle();

	UFUNCTION(BlueprintPure, Category = "Puzzle")
	EPuzzleState GetPuzzleState() const { return CurrentState; }

	UFUNCTION(BlueprintPure, Category = "Puzzle")
	bool IsSolved() const { return CurrentState == EPuzzleState::Solved; }

	UFUNCTION(BlueprintPure, Category = "Puzzle")
	bool IsActive() const { return CurrentState == EPuzzleState::Active; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> InteractionVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	EPuzzleState CurrentState = EPuzzleState::Inactive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	bool bAutoActivateOnBeginPlay = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	bool bCanBeReset = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	TArray<TObjectPtr<APuzzleBase>> LinkedPuzzles;

	UPROPERTY(BlueprintAssignable, Category = "Puzzle Events")
	FPuzzleStateChanged OnPuzzleStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Puzzle Events")
	FPuzzleSolved OnPuzzleSolved;

	virtual void OnInteract_Implementation(AMinaCharacter* Interactor);
	virtual void OnActivate_Implementation();
	virtual void OnDeactivate_Implementation();
	virtual void OnSolve_Implementation();
	virtual void OnFail_Implementation();
	virtual void OnReset_Implementation();

	void SetState(EPuzzleState NewState);
	void NotifyLinkedPuzzles();
};