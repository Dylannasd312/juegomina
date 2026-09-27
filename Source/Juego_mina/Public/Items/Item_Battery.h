#pragma once

#include "CoreMinimal.h"
#include "Items/ItemBase.h"
#include "Item_Battery.generated.h"

class AMinaCharacter;

UCLASS(Blueprintable, BlueprintType)
class JUEGO_MINA_API AItem_Battery : public AItemBase
{
	GENERATED_BODY()

public:
	AItem_Battery();

public:
	UFUNCTION(BlueprintCallable, Category = "Battery")
	virtual void Use(AMinaCharacter* Character) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battery")
	float ChargeAmount = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battery")
	bool bRechargeFlashlight = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battery")
	bool bRestoreStamina = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battery")
	float StaminaRestoreAmount = 20.0f;
};