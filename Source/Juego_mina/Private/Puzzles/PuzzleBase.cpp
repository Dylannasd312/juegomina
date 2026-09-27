#include "Puzzles/PuzzleBase.h"
#include "Character/MinaCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

APuzzleBase::APuzzleBase()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	InteractionVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionVolume->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
}

void APuzzleBase::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoActivateOnBeginPlay)
	{
		ActivatePuzzle();
	}
}

void APuzzleBase::Interact(AMinaCharacter* Interactor)
{
	if (CurrentState == EPuzzleState::Active)
	{
		OnInteract_Implementation(Interactor);
	}
}

void APuzzleBase::ActivatePuzzle()
{
	if (CurrentState == EPuzzleState::Inactive || CurrentState == EPuzzleState::Failed)
	{
		SetState(EPuzzleState::Active);
		OnActivate_Implementation();
	}
}

void APuzzleBase::DeactivatePuzzle()
{
	if (CurrentState == EPuzzleState::Active)
	{
		SetState(EPuzzleState::Inactive);
		OnDeactivate_Implementation();
	}
}

void APuzzleBase::SolvePuzzle()
{
	if (CurrentState == EPuzzleState::Active)
	{
		SetState(EPuzzleState::Solved);
		OnSolve_Implementation();
		OnPuzzleSolved.Broadcast();
		NotifyLinkedPuzzles();
	}
}

void APuzzleBase::FailPuzzle()
{
	if (CurrentState == EPuzzleState::Active)
	{
		SetState(EPuzzleState::Failed);
		OnFail_Implementation();
	}
}

void APuzzleBase::ResetPuzzle()
{
	if (bCanBeReset && (CurrentState == EPuzzleState::Solved || CurrentState == EPuzzleState::Failed))
	{
		SetState(EPuzzleState::Inactive);
		OnReset_Implementation();
	}
}

void APuzzleBase::OnInteract_Implementation(AMinaCharacter* Interactor)
{
}

void APuzzleBase::OnActivate_Implementation()
{
}

void APuzzleBase::OnDeactivate_Implementation()
{
}

void APuzzleBase::OnSolve_Implementation()
{
}

void APuzzleBase::OnFail_Implementation()
{
}

void APuzzleBase::OnReset_Implementation()
{
}

void APuzzleBase::SetState(EPuzzleState NewState)
{
	CurrentState = NewState;
	OnPuzzleStateChanged.Broadcast(NewState);
}

void APuzzleBase::NotifyLinkedPuzzles()
{
	for (APuzzleBase* LinkedPuzzle : LinkedPuzzles)
	{
		if (LinkedPuzzle && LinkedPuzzle->GetPuzzleState() == EPuzzleState::Inactive)
		{
			LinkedPuzzle->ActivatePuzzle();
		}
	}
}