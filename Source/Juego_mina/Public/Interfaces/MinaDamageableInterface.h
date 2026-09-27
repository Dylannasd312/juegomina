#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MinaDamageableInterface.generated.h"

class AActor;

UINTERFACE(BlueprintType, MinimalAPI)
class UMinaDamageableInterface : public UInterface
{
	GENERATED_BODY()
};

class JUEGO_MINA_API IMinaDamageableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
	void TakeDamage(float DamageAmount, AActor* DamageCauser, const FHitResult& HitResult);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
	float GetHealth() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
	bool IsDead() const;
};