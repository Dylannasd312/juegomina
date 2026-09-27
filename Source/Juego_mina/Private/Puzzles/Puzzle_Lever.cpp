#include "Puzzles/Puzzle_Lever.h"
#include "Character/MinaCharacter.h"
#include "Components/TimelineComponent.h"
#include "Components/AudioComponent.h"
#include "Curves/CurveFloat.h"
#include "Kismet/GameplayStatics.h"

APuzzle_Lever::APuzzle_Lever()
{
	PrimaryActorTick.bCanEverTick = true;

	LeverTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("LeverTimeline"));

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);
	AudioComponent->bAutoActivate = false;
}

void APuzzle_Lever::BeginPlay()
{
	Super::BeginPlay();

	RestRotation = GetActorRotation();
	PulledRotation = RestRotation + FRotator(0.0f, 0.0f, PullAngle);

	if (LeverCurve && LeverTimeline)
	{
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this, FName("UpdateLever"));
		LeverTimeline->AddInterpFloat(LeverCurve, TimelineCallback);
		LeverTimeline->SetTimelineLength(LeverCurve->GetFloatValue(LeverCurve->GetKeys().Last().Time));
	}
}

void APuzzle_Lever::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (LeverTimeline)
	{
		LeverTimeline->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, nullptr);
	}
}

void APuzzle_Lever::PullLever()
{
	if (!bIsPulled && LeverTimeline)
	{
		bIsPulled = true;
		LeverTimeline->PlayFromStart();
		PlayLeverSound(PullSound);
		SolvePuzzle();
	}
}

void APuzzle_Lever::ResetLever()
{
	if (bIsPulled && LeverTimeline)
	{
		bIsPulled = false;
		LeverTimeline->ReverseFromEnd();
		PlayLeverSound(ResetSound);
	}
}

void APuzzle_Lever::OnInteract_Implementation(AMinaCharacter* Interactor)
{
	if (CurrentState == EPuzzleState::Active || CurrentState == EPuzzleState::Inactive)
	{
		ActivatePuzzle();
		PullLever();
	}
}

void APuzzle_Lever::OnSolve_Implementation()
{
	if (bAutoReset)
	{
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &APuzzle_Lever::ResetLever, AutoResetDelay, false);
	}
}

void APuzzle_Lever::OnReset_Implementation()
{
	ResetLever();
	if (bAutoReset)
	{
		ResetPuzzle();
	}
}

void APuzzle_Lever::UpdateLever(float Value)
{
	FRotator NewRotation = FMath::Lerp(RestRotation, PulledRotation, Value);
	SetActorRotation(NewRotation);
}

void APuzzle_Lever::PlayLeverSound(TObjectPtr<USoundBase> Sound)
{
	if (Sound && AudioComponent)
	{
		AudioComponent->SetSound(Sound);
		AudioComponent->Play();
	}
}