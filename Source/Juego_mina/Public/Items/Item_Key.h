#pragma once

#include "CoreMinimal.h"
#include "Items/ItemBase.h"
#include "Item_Key.generated.h"

class AMinaCharacter;

UCLASS(Blueprintable, BlueprintType)
class JUEGO_MINA_API AItem_Key : public AItemBase
{
	GENERATED_BODY()

public:
	AItem_Key();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Key")
	virtual void Pickup(AMinaCharacter* Character) override;

	UFUNCTION(BlueprintPure, Category = "Key")
	FGameplayTag GetKeyTag() const { return KeyTag; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key")
	FGameplayTag KeyTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key")
	bool bIsUnique = true;
};