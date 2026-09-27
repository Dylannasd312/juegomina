#pragma once

#include "CoreMinimal.h"
#include "Puzzles/PuzzleBase.h"
#include "Puzzle_Lever.generated.h"

class UTimelineComponent;
class UCurveFloat;
class UAudioComponent;

UCLASS(Blueprintable, BlueprintType)
class JUEGO_MINA_API APuzzle_Lever : public APuzzleBase
{
	GENERATED_BODY()

public:
	APuzzle_Lever();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Lever")
	void PullLever();

	UFUNCTION(BlueprintCallable, Category = "Lever")
	void ResetLever();

	UFUNCTION(BlueprintPure, Category = "Lever")
	bool IsPulled() const { return bIsPulled; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lever")
	float PullAngle = -90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lever")
	float PullSpeed = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lever")
	bool bAutoReset = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lever")
	float AutoResetDelay = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lever|Audio")
	TObjectPtr<USoundBase> PullSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lever|Audio")
	TObjectPtr<USoundBase> ResetSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lever")
	TObjectPtr<UAudioComponent> AudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lever")
	TObjectPtr<UTimelineComponent> LeverTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lever")
	TObjectPtr<UCurveFloat> LeverCurve;

private:
	bool bIsPulled = false;
	FRotator RestRotation;
	FRotator PulledRotation;

	virtual void OnInteract_Implementation(AMinaCharacter* Interactor) override;
	virtual void OnSolve_Implementation() override;
	virtual void OnReset_Implementation() override;

	void UpdateLever(float Value);
	void PlayLeverSound(TObjectPtr<USoundBase> Sound);
};