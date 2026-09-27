#pragma once

#include "CoreMinimal.h"
#include "Puzzles/PuzzleBase.h"
#include "Puzzle_Door.generated.h"

class UTimelineComponent;
class UCurveFloat;
class UAudioComponent;

UCLASS(Blueprintable, BlueprintType)
class JUEGO_MINA_API APuzzle_Door : public APuzzleBase
{
	GENERATED_BODY()

public:
	APuzzle_Door();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Door")
	void OpenDoor();

	UFUNCTION(BlueprintCallable, Category = "Door")
	void CloseDoor();

	UFUNCTION(BlueprintCallable, Category = "Door")
	void ToggleDoor();

	UFUNCTION(BlueprintCallable, Category = "Door")
	void SetRequiresKey(bool bRequiresKey);

	UFUNCTION(BlueprintCallable, Category = "Door")
	void SetKeyTag(FGameplayTag NewKeyTag);

	UFUNCTION(BlueprintPure, Category = "Door")
	bool IsOpen() const { return bIsOpen; }

	UFUNCTION(BlueprintPure, Category = "Door")
	bool RequiresKey() const { return bRequiresKey; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	bool bRequiresKey = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	FGameplayTag RequiredKeyTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float OpenAngle = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float OpenSpeed = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	bool bStartOpen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Audio")
	TObjectPtr<USoundBase> OpenSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Audio")
	TObjectPtr<USoundBase> CloseSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Audio")
	TObjectPtr<USoundBase> LockedSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	TObjectPtr<UAudioComponent> AudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	TObjectPtr<UTimelineComponent> DoorTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	TObjectPtr<UCurveFloat> DoorCurve;

private:
	bool bIsOpen = false;
	FRotator ClosedRotation;
	FRotator OpenRotation;

	virtual void OnInteract_Implementation(AMinaCharacter* Interactor) override;
	virtual void OnActivate_Implementation() override;
	virtual void OnSolve_Implementation() override;
	virtual void OnReset_Implementation() override;

	void UpdateDoor(float Value);
	void PlayDoorSound(TObjectPtr<USoundBase> Sound);
	bool HasRequiredKey(AMinaCharacter* Interactor) const;
};