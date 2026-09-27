#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MinaInteractableInterface.generated.h"

class AMinaCharacter;

UINTERFACE(BlueprintType, MinimalAPI)
class UMinaInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class JUEGO_MINA_API IMinaInteractableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void OnInteract(AMinaCharacter* Interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	FText GetInteractionText() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	bool CanInteract(AMinaCharacter* Interactor) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void OnFocusGained(AMinaCharacter* Interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void OnFocusLost(AMinaCharacter* Interactor);
};