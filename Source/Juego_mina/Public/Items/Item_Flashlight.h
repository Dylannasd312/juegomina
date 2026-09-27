#pragma once

#include "CoreMinimal.h"
#include "Items/ItemBase.h"
#include "Item_Flashlight.generated.h"

class AMinaCharacter;

UCLASS(Blueprintable, BlueprintType)
class JUEGO_MINA_API AItem_Flashlight : public AItemBase
{
	GENERATED_BODY()

public:
	AItem_Flashlight();

public:
	UFUNCTION(BlueprintCallable, Category = "Flashlight")
	virtual void Pickup(AMinaCharacter* Character) override;

	UFUNCTION(BlueprintCallable, Category = "Flashlight")
	virtual void Use(AMinaCharacter* Character) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight")
	float BatteryLife = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight")
	float MaxBatteryLife = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight")
	float DrainRate = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight")
	bool bStartEnabled = false;
};