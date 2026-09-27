#include "AI/MinaEnemyAI.h"
#include "GameplayStateTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"

AMinaEnemyAI::AMinaEnemyAI()
{
	PrimaryActorTick.bCanEverTick = true;

	StateTreeComponent = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponent"));

	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SetPerceptionComponent(*PerceptionComponent);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;

	CurrentHealth = MaxHealth;
}

void AMinaEnemyAI::BeginPlay()
{
	Super::BeginPlay();

	InitializePerception();

	if (StateTreeComponent && StateTreeAsset)
	{
		StateTreeComponent->StartStateTree(*StateTreeAsset);
	}

	if (AController* Controller = GetController())
	{
		Controller->ReceiveMessage(TEXT("OnPossess"), this, true);
	}
}

void AMinaEnemyAI::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (StateTreeComponent && StateTreeAsset)
	{
		StateTreeComponent->StartStateTree(*StateTreeAsset);
	}
}

void AMinaEnemyAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateEnemyState();
}

void AMinaEnemyAI::InitializePerception()
{
	if (!PerceptionComponent)
	{
		return;
	}

	UAISenseConfig_Sight* SightConfig = NewObject<UAISenseConfig_Sight>(this);
	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngle;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->SetMaxAge(5.0f);

	UAISenseConfig_Hearing* HearingConfig = NewObject<UAISenseConfig_Hearing>(this);
	HearingConfig->HearingRange = HearingRange;
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->SetMaxAge(10.0f);

	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->ConfigureSense(*HearingConfig);
	PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AMinaEnemyAI::OnTargetPerceptionUpdated);
}

void AMinaEnemyAI::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor)
	{
		return;
	}

	APawn* PlayerPawn = Cast<APawn>(Actor);
	if (!PlayerPawn)
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		LastKnownPlayerLocation = PlayerPawn;
		OnSeePlayer(PlayerPawn);
	}
	else
	{
		if (LastKnownPlayerLocation == PlayerPawn)
		{
			LastKnownPlayerLocation = nullptr;
		}
	}
}

void AMinaEnemyAI::SetEnemyState(EEnemyState NewState)
{
	if (CurrentEnemyState != NewState)
	{
		CurrentEnemyState = NewState;

		if (StateTreeComponent && StateTreeComponent->GetStateTree())
		{
			StateTreeComponent->SetState(NewState);
		}
	}
}

void AMinaEnemyAI::SetPatrolPoints(const TArray<AActor*>& NewPatrolPoints)
{
	PatrolPoints = NewPatrolPoints;
	CurrentPatrolIndex = 0;
}

void AMinaEnemyAI::OnSeePlayer(APawn* PlayerPawn)
{
	if (CurrentEnemyState != EEnemyState::Chase && CurrentEnemyState != EEnemyState::Attack)
	{
		SetEnemyState(EEnemyState::Chase);
	}
}

void AMinaEnemyAI::OnHearNoise(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	if (CurrentEnemyState == EEnemyState::Idle || CurrentEnemyState == EEnemyState::Patrol)
	{
		SetEnemyState(EEnemyState::Investigate);
	}
}

void AMinaEnemyAI::TakeDamage(float DamageAmount, AActor* DamageCauser)
{
	CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);

	if (CurrentHealth <= 0.0f)
	{
		Die();
	}
	else if (CurrentHealth / MaxHealth <= 0.3f)
	{
		SetEnemyState(EEnemyState::Flee);
	}
}

void AMinaEnemyAI::Die()
{
	SetEnemyState(EEnemyState::Dead);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCharacterMovement()->DisableMovement();

	if (AController* Controller = GetController())
	{
		Controller->UnPossess();
	}

	SetLifeSpan(10.0f);
}

void AMinaEnemyAI::UpdateEnemyState()
{
	switch (CurrentEnemyState)
	{
	case EEnemyState::Idle:
		if (PatrolPoints.Num() > 0)
		{
			SetEnemyState(EEnemyState::Patrol);
		}
		break;

	case EEnemyState::Patrol:
		HandlePatrol();
		break;

	case EEnemyState::Investigate:
		if (LastKnownPlayerLocation)
		{
			SetEnemyState(EEnemyState::Chase);
		}
		break;

	case EEnemyState::Chase:
		if (LastKnownPlayerLocation)
		{
			float Distance = FVector::Dist(GetActorLocation(), LastKnownPlayerLocation->GetActorLocation());
			if (Distance <= AttackRange)
			{
				SetEnemyState(EEnemyState::Attack);
			}
		}
		else
		{
			SetEnemyState(EEnemyState::Investigate);
		}
		break;

	case EEnemyState::Attack:
		HandleAttack();
		break;

	case EEnemyState::Flee:
		break;

	case EEnemyState::Dead:
		break;
	}
}

void AMinaEnemyAI::HandlePatrol()
{
	if (PatrolPoints.Num() == 0)
	{
		return;
	}

	AActor* TargetPoint = PatrolPoints[CurrentPatrolIndex];
	if (!TargetPoint)
	{
		return;
	}

	float Distance = FVector::Dist(GetActorLocation(), TargetPoint->GetActorLocation());
	if (Distance <= 100.0f)
	{
		if (bRandomPatrol)
		{
			CurrentPatrolIndex = FMath::RandRange(0, PatrolPoints.Num() - 1);
		}
		else
		{
			CurrentPatrolIndex = (CurrentPatrolIndex + 1) % PatrolPoints.Num();
		}
	}

	if (AController* Controller = GetController())
	{
		Controller->MoveToActor(TargetPoint, 50.0f);
	}
}

void AMinaEnemyAI::HandleAttack()
{
	if (!LastKnownPlayerLocation)
	{
		SetEnemyState(EEnemyState::Chase);
		return;
	}

	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastAttackTime >= AttackCooldown)
	{
		LastAttackTime = CurrentTime;

		FVector Direction = (LastKnownPlayerLocation->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		SetActorRotation(Direction.Rotation());

		UGameplayStatics::ApplyDamage(LastKnownPlayerLocation, Damage, GetController(), this, nullptr);
	}
}