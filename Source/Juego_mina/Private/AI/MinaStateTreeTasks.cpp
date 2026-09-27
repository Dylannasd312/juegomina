#include "AI/MinaStateTreeTasks.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameplayTagsModule.h"

EStateTreeTaskStatus FMinaMoveToTargetTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	AAIController* AIController = Cast<AAIController>(Context.GetActor());
	if (!AIController)
	{
		return EStateTreeTaskStatus::Failed;
	}

	AActor* TargetActor = nullptr;
	if (Context.GetData<TObjectPtr<AActor>>(TargetActorPath, TargetActor) && TargetActor)
	{
		FAIMoveRequest MoveRequest(TargetActor);
		MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
		MoveRequest.SetStopOnOverlap(bStopOnOverlap);

		FNavPathSharedPtr Path;
		EPathFollowingRequestResult::Type Result = AIController->MoveTo(MoveRequest, &Path);

		if (Result == EPathFollowingRequestResult::RequestSuccessful || Result == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			return EStateTreeTaskStatus::Running;
		}
	}

	return EStateTreeTaskStatus::Failed;
}

EStateTreeTaskStatus FMinaMoveToTargetTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	AAIController* AIController = Cast<AAIController>(Context.GetActor());
	if (!AIController)
	{
		return EStateTreeTaskStatus::Failed;
	}

	EPathFollowingStatus::Type Status = AIController->GetMoveStatus();
	switch (Status)
	{
	case EPathFollowingStatus::Moving:
		return EStateTreeTaskStatus::Running;
	case EPathFollowingStatus::Idle:
		return EStateTreeTaskStatus::Succeeded;
	case EPathFollowingStatus::Paused:
		return EStateTreeTaskStatus::Running;
	default:
		return EStateTreeTaskStatus::Failed;
	}
}

void FMinaMoveToTargetTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	AAIController* AIController = Cast<AAIController>(Context.GetActor());
	if (AIController)
	{
		AIController->StopMovement();
	}
}

EStateTreeTaskStatus FMinaWaitTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	WaitTimeRemaining = FMath::FRandRange(MinWaitTime, MaxWaitTime);
	return EStateTreeTaskStatus::Running;
}

EStateTreeTaskStatus FMinaWaitTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	WaitTimeRemaining -= DeltaTime;
	if (WaitTimeRemaining <= 0.0f)
	{
		return EStateTreeTaskStatus::Succeeded;
	}
	return EStateTreeTaskStatus::Running;
}

EStateTreeTaskStatus FMinaPlayMontageTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	ACharacter* Character = Cast<ACharacter>(Context.GetActor());
	if (!Character)
	{
		return EStateTreeTaskStatus::Failed;
	}

	UAnimMontage* Montage = nullptr;
	if (Context.GetData<UAnimMontage*>(MontagePath, Montage) && Montage)
	{
		UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(Montage, PlayRate);
			if (StartSectionName != NAME_None)
			{
				AnimInstance->Montage_JumpToSection(StartSectionName, Montage);
			}
			bMontageStarted = true;
			return EStateTreeTaskStatus::Running;
		}
	}

	return EStateTreeTaskStatus::Failed;
}

EStateTreeTaskStatus FMinaPlayMontageTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	if (!bMontageStarted)
	{
		return EStateTreeTaskStatus::Failed;
	}

	ACharacter* Character = Cast<ACharacter>(Context.GetActor());
	if (!Character)
	{
		return EStateTreeTaskStatus::Failed;
	}

	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return EStateTreeTaskStatus::Failed;
	}

	UAnimMontage* Montage = nullptr;
	Context.GetData<UAnimMontage*>(MontagePath, Montage);

	if (Montage && AnimInstance->Montage_IsPlaying(Montage))
	{
		return EStateTreeTaskStatus::Running;
	}

	return EStateTreeTaskStatus::Succeeded;
}

void FMinaPlayMontageTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	ACharacter* Character = Cast<ACharacter>(Context.GetActor());
	if (Character)
	{
		UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
		UAnimMontage* Montage = nullptr;
		Context.GetData<UAnimMontage*>(MontagePath, Montage);

		if (AnimInstance && Montage && AnimInstance->Montage_IsPlaying(Montage))
		{
			AnimInstance->Montage_Stop(0.2f, Montage);
		}
	}
	bMontageStarted = false;
}

bool FMinaCheckLOS::TestCondition(FStateTreeExecutionContext& Context) const
{
	AActor* SelfActor = Context.GetActor();
	AActor* TargetActor = nullptr;

	if (!Context.GetData<TObjectPtr<AActor>>(TargetActorPath, TargetActor) || !TargetActor || !SelfActor)
	{
		return false;
	}

	FVector Start = SelfActor->GetActorLocation();
	FVector End = TargetActor->GetActorLocation();

	if (FVector::Dist(Start, End) > MaxDistance)
	{
		return false;
	}

	if (TraceObjectTypes.Num() == 0)
	{
		TraceObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);
		TraceObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery2);
		TraceObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery3);
	}

	TArray<AActor*> ActorsToIgnore = { SelfActor, TargetActor };
	FHitResult HitResult;

	bool bHit = UKismetSystemLibrary::LineTraceMultiForObjects(
		SelfActor->GetWorld(),
		Start,
		End,
		TraceObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true
	);

	return !bHit;
}

bool FMinaCheckDistance::TestCondition(FStateTreeExecutionContext& Context) const
{
	AActor* SelfActor = Context.GetActor();
	AActor* TargetActor = nullptr;

	if (!Context.GetData<TObjectPtr<AActor>>(TargetActorPath, TargetActor) || !TargetActor || !SelfActor)
	{
		return false;
	}

	float Distance = bUse2DDistance
		? FVector::Dist2D(SelfActor->GetActorLocation(), TargetActor->GetActorLocation())
		: FVector::Dist(SelfActor->GetActorLocation(), TargetActor->GetActorLocation());

	return Distance <= MaxDistance;
}

bool FMinaHealthCheck::TestCondition(FStateTreeExecutionContext& Context) const
{
	ACharacter* Character = Cast<ACharacter>(Context.GetActor());
	if (!Character)
	{
		return false;
	}

	float Health = 100.0f;
	Character->GetGameplayTagCount(FGameplayTag::RequestGameplayTag("Attribute.Health"), Health);

	float MaxHealth = 100.0f;
	Character->GetGameplayTagCount(FGameplayTag::RequestGameplayTag("Attribute.MaxHealth"), MaxHealth);

	if (MaxHealth > 0.0f)
	{
		return (Health / MaxHealth) <= HealthThreshold;
	}

	return false;
}