#pragma once

#include "CoreMinimal.h"
#include "GameplayStateTree.h"
#include "AIController.h"
#include "MinaStateTreeTasks.generated.h"

class UStateTreeComponent;
class AAIController;
class APawn;

USTRUCT(DisplayName = "Move To Target", Category = "Mina AI")
struct FMinaMoveToTargetTask : public FStateTreeTask
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Move To Target")
	FStateTreePropertyPath TargetActorPath;

	UPROPERTY(EditAnywhere, Category = "Move To Target")
	float AcceptanceRadius = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Move To Target")
	bool bStopOnOverlap = false;

	virtual EStateTreeTaskStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeTaskStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};

USTRUCT(DisplayName = "Wait", Category = "Mina AI")
struct FMinaWaitTask : public FStateTreeTask
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Wait")
	float MinWaitTime = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Wait")
	float MaxWaitTime = 3.0f;

	mutable float WaitTimeRemaining = 0.0f;

	virtual EStateTreeTaskStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeTaskStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};

USTRUCT(DisplayName = "Play Animation Montage", Category = "Mina AI")
struct FMinaPlayMontageTask : public FStateTreeTask
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Animation")
	FStateTreePropertyPath MontagePath;

	UPROPERTY(EditAnywhere, Category = "Animation")
	float PlayRate = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Animation")
	FName StartSectionName = NAME_None;

	mutable bool bMontageStarted = false;

	virtual EStateTreeTaskStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeTaskStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};

USTRUCT(DisplayName = "Check Line of Sight", Category = "Mina AI")
struct FMinaCheckLOS : public FStateTreeCondition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "LOS")
	FStateTreePropertyPath TargetActorPath;

	UPROPERTY(EditAnywhere, Category = "LOS")
	float MaxDistance = 2000.0f;

	UPROPERTY(EditAnywhere, Category = "LOS")
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};

USTRUCT(DisplayName = "Check Distance", Category = "Mina AI")
struct FMinaCheckDistance : public FStateTreeCondition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Distance")
	FStateTreePropertyPath TargetActorPath;

	UPROPERTY(EditAnywhere, Category = "Distance")
	float MaxDistance = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Distance")
	bool bUse2DDistance = false;

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};

USTRUCT(DisplayName = "Health Check", Category = "Mina AI")
struct FMinaHealthCheck : public FStateTreeCondition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Health")
	float HealthThreshold = 0.3f;

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};