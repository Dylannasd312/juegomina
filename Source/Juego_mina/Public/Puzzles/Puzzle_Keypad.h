#pragma once

#include "CoreMinimal.h"
#include "Puzzles/PuzzleBase.h"
#include "Puzzle_Keypad.generated.h"

class UTextRenderComponent;
class UAudioComponent;

UCLASS(Blueprintable, BlueprintType)
class JUEGO_MINA_API APuzzle_Keypad : public APuzzleBase
{
	GENERATED_BODY()

public:
	APuzzle_Keypad();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Keypad")
	void PressKey(int32 KeyIndex);

	UFUNCTION(BlueprintCallable, Category = "Keypad")
	void SubmitCode();

	UFUNCTION(BlueprintCallable, Category = "Keypad")
	void ClearInput();

	UFUNCTION(BlueprintCallable, Category = "Keypad")
	void SetCode(const FString& NewCode);

	UFUNCTION(BlueprintPure, Category = "Keypad")
	FString GetCurrentInput() const { return CurrentInput; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keypad")
	FString CorrectCode = TEXT("1234");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keypad")
	int32 MaxCodeLength = 4;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Keypad")
	FString CurrentInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keypad")
	TObjectPtr<UTextRenderComponent> DisplayText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keypad|Audio")
	TObjectPtr<USoundBase> KeyPressSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keypad|Audio")
	TObjectPtr<USoundBase> SuccessSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keypad|Audio")
	TObjectPtr<USoundBase> FailSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Keypad|Audio")
	TObjectPtr<UAudioComponent> AudioComponent;

	virtual void OnInteract_Implementation(AMinaCharacter* Interactor) override;
	virtual void OnActivate_Implementation() override;
	virtual void OnSolve_Implementation() override;
	virtual void OnFail_Implementation() override;
	virtual void OnReset_Implementation() override;

	void UpdateDisplay();
	void PlaySound(TObjectPtr<USoundBase> Sound);
};