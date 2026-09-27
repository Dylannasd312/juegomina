#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayStateTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "MinaEnemyAI.generated.h"

class UStateTreeComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UBehaviorTree;
class UBlackboardData;

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle,
	Patrol,
	Investigate,
	Chase,
	Attack,
	Flee,
	Dead
};

UCLASS(Blueprintable, BlueprintType)
class JUEGO_MINA_API AMinaEnemyAI : public ACharacter
{
	GENERATED_BODY()

public:
	AMinaEnemyAI();

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Enemy AI")
	void SetEnemyState(EEnemyState NewState);

	UFUNCTION(BlueprintCallable, Category = "Enemy AI")
	EEnemyState GetEnemyState() const { return CurrentEnemyState; }

	UFUNCTION(BlueprintCallable, Category = "Enemy AI")
	void SetPatrolPoints(const TArray<AActor*>& NewPatrolPoints);

	UFUNCTION(BlueprintCallable, Category = "Enemy AI")
	void OnSeePlayer(APawn* PlayerPawn);

	UFUNCTION(BlueprintCallable, Category = "Enemy AI")
	void OnHearNoise(APawn* NoiseInstigator, const FVector& Location, float Volume);

	UFUNCTION(BlueprintCallable, Category = "Enemy AI")
	void TakeDamage(float DamageAmount, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable, Category = "Enemy AI")
	void Die();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UStateTreeComponent> StateTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAIPerceptionComponent> PerceptionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<UStateTree> StateTreeAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<UBlackboardData> BlackboardAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Sight")
	float SightRadius = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Sight")
	float LoseSightRadius = 2500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Sight")
	float PeripheralVisionAngle = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Hearing")
	float HearingRange = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Stats")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Stats")
	float CurrentHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Stats")
	float Damage = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Stats")
	float AttackRange = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Stats")
	float AttackCooldown = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Patrol")
	TArray<TObjectPtr<AActor>> PatrolPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Patrol")
	bool bRandomPatrol = false;

private:
	EEnemyState CurrentEnemyState = EEnemyState::Idle;
	APawn* LastKnownPlayerLocation = nullptr;
	float LastAttackTime = 0.0f;
	int32 CurrentPatrolIndex = 0;

	void InitializePerception();
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	void UpdateEnemyState();
	void HandlePatrol();
	void HandleAttack();
};