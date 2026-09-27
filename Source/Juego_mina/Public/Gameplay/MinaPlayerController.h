#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "MinaPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class AMinaCharacter;

UCLASS(Blueprintable, BlueprintType)
class JUEGO_MINA_API AMinaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMinaPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Input")
	void SetInputMappingContext(TObjectPtr<UInputMappingContext> NewContext, int32 Priority = 0);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void RemoveInputMappingContext(TObjectPtr<UInputMappingContext> Context);

	UFUNCTION(BlueprintCallable, Category = "Pause")
	void TogglePause();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void RestartLevel();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputMappingContext> GameplayMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputMappingContext> UIMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> PauseAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> InventoryAction;

private:
	bool bIsPaused = false;

	void OnPause(const FInputActionValue& Value);
	void OnInteract(const FInputActionValue& Value);
	void OnInventory(const FInputActionValue& Value);
};