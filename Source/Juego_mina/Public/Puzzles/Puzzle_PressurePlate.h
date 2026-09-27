#pragma once

#include "CoreMinimal.h"
#include "Puzzles/PuzzleBase.h"
#include "Puzzle_PressurePlate.generated.h"

class UBoxComponent;
class UAudioComponent;

UCLASS(Blueprintable, BlueprintType)
class JUEGO_MINA_API APuzzle_PressurePlate : public APuzzleBase
{
	GENERATED_BODY()

public:
	APuzzle_PressurePlate();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "PressurePlate")
	void SetRequiredWeight(float Weight);

	UFUNCTION(BlueprintCallable, Category = "PressurePlate")
	void SetRequiredTag(FGameplayTag Tag);

	UFUNCTION(BlueprintPure, Category = "PressurePlate")
	bool IsActivated() const { return bIsActivated; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PressurePlate")
	float RequiredWeight = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PressurePlate")
	FGameplayTag RequiredActorTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PressurePlate")
	bool bRequireContinuousPressure = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PressurePlate|Visual")
	TObjectPtr<UMaterialInterface> ActiveMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PressurePlate|Visual")
	TObjectPtr<UMaterialInterface> InactiveMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PressurePlate|Audio")
	TObjectPtr<USoundBase> ActivateSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PressurePlate|Audio")
	TObjectPtr<USoundBase> DeactivateSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PressurePlate")
	TObjectPtr<UBoxComponent> TriggerVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PressurePlate")
	TObjectPtr<UAudioComponent> AudioComponent;

private:
	bool bIsActivated = false;
	TArray<AActor*> OverlappingActors;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void OnActivate_Implementation() override;
	virtual void OnDeactivate_Implementation() override;

	void CheckActivation();
	void UpdateVisuals();
	void PlayPlateSound(TObjectPtr<USoundBase> Sound);
};