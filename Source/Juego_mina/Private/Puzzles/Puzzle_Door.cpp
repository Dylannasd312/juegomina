#include "Puzzles/Puzzle_Door.h"
#include "Character/MinaCharacter.h"
#include "Components/TimelineComponent.h"
#include "Components/AudioComponent.h"
#include "Curves/CurveFloat.h"
#include "Kismet/GameplayStatics.h"

APuzzle_Door::APuzzle_Door()
{
	PrimaryActorTick.bCanEverTick = true;

	DoorTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DoorTimeline"));

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);
	AudioComponent->bAutoActivate = false;
}

void APuzzle_Door::BeginPlay()
{
	Super::BeginPlay();

	ClosedRotation = GetActorRotation();
	OpenRotation = ClosedRotation + FRotator(0.0f, OpenAngle, 0.0f);

	if (DoorCurve && DoorTimeline)
	{
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this, FName("UpdateDoor"));
		DoorTimeline->AddInterpFloat(DoorCurve, TimelineCallback);
		DoorTimeline->SetTimelineLength(DoorCurve->GetFloatValue(DoorCurve->GetKeys().Last().Time));
	}

	if (bStartOpen)
	{
		bIsOpen = true;
		SetActorRotation(OpenRotation);
	}
}

void APuzzle_Door::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DoorTimeline)
	{
		DoorTimeline->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, nullptr);
	}
}

void APuzzle_Door::OpenDoor()
{
	if (!bIsOpen && DoorTimeline)
	{
		bIsOpen = true;
		DoorTimeline->PlayFromStart();
		PlayDoorSound(OpenSound);
	}
}

void APuzzle_Door::CloseDoor()
{
	if (bIsOpen && DoorTimeline)
	{
		bIsOpen = false;
		DoorTimeline->ReverseFromEnd();
		PlayDoorSound(CloseSound);
	}
}

void APuzzle_Door::ToggleDoor()
{
	if (bIsOpen)
	{
		CloseDoor();
	}
	else
	{
		OpenDoor();
	}
}

void APuzzle_Door::SetRequiresKey(bool bNewRequiresKey)
{
	bRequiresKey = bNewRequiresKey;
}

void APuzzle_Door::SetKeyTag(FGameplayTag NewKeyTag)
{
	RequiredKeyTag = NewKeyTag;
}

void APuzzle_Door::OnInteract_Implementation(AMinaCharacter* Interactor)
{
	if (bRequiresKey && !HasRequiredKey(Interactor))
	{
		PlayDoorSound(LockedSound);
		return;
	}

	if (CurrentState == EPuzzleState::Active || CurrentState == EPuzzleState::Inactive)
	{
		ActivatePuzzle();
		ToggleDoor();
	}
}

void APuzzle_Door::OnActivate_Implementation()
{
}

void APuzzle_Door::OnSolve_Implementation()
{
	OpenDoor();
}

void APuzzle_Door::OnReset_Implementation()
{
	if (bIsOpen)
	{
		CloseDoor();
	}
}

void APuzzle_Door::UpdateDoor(float Value)
{
	FRotator NewRotation = FMath::Lerp(ClosedRotation, OpenRotation, Value);
	SetActorRotation(NewRotation);
}

void APuzzle_Door::PlayDoorSound(TObjectPtr<USoundBase> Sound)
{
	if (Sound && AudioComponent)
	{
		AudioComponent->SetSound(Sound);
		AudioComponent->Play();
	}
}

bool APuzzle_Door::HasRequiredKey(AMinaCharacter* Interactor) const
{
	if (!Interactor || !RequiredKeyTag.IsValid())
	{
		return false;
	}

	return Interactor->GetGameplayTagCount(RequiredKeyTag) > 0;
}