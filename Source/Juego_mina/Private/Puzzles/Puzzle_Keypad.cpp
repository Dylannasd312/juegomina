#include "Puzzles/Puzzle_Keypad.h"
#include "Character/MinaCharacter.h"
#include "Components/TextRenderComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

APuzzle_Keypad::APuzzle_Keypad()
{
	DisplayText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("DisplayText"));
	DisplayText->SetupAttachment(MeshComponent);
	DisplayText->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	DisplayText->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	DisplayText->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
	DisplayText->VerticalAlignment = EVerticalTextAligment::EVRTA_TextCenter;
	DisplayText->WorldSize = 20.0f;

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);
	AudioComponent->bAutoActivate = false;
}

void APuzzle_Keypad::BeginPlay()
{
	Super::BeginPlay();
	UpdateDisplay();
}

void APuzzle_Keypad::PressKey(int32 KeyIndex)
{
	if (CurrentState != EPuzzleState::Active || CurrentInput.Len() >= MaxCodeLength)
	{
		return;
	}

	CurrentInput.AppendInt(KeyIndex);
	UpdateDisplay();
	PlaySound(KeyPressSound);
}

void APuzzle_Keypad::SubmitCode()
{
	if (CurrentState != EPuzzleState::Active)
	{
		return;
	}

	if (CurrentInput == CorrectCode)
	{
		SolvePuzzle();
		PlaySound(SuccessSound);
	}
	else
	{
		FailPuzzle();
		PlaySound(FailSound);
	}
}

void APuzzle_Keypad::ClearInput()
{
	CurrentInput.Empty();
	UpdateDisplay();
}

void APuzzle_Keypad::SetCode(const FString& NewCode)
{
	CorrectCode = NewCode;
}

void APuzzle_Keypad::OnInteract_Implementation(AMinaCharacter* Interactor)
{
}

void APuzzle_Keypad::OnActivate_Implementation()
{
	ClearInput();
	UpdateDisplay();
}

void APuzzle_Keypad::OnSolve_Implementation()
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TEXT("ACCESS GRANTED")));
		DisplayText->SetTextRenderColor(FColor::Green);
	}
}

void APuzzle_Keypad::OnFail_Implementation()
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TEXT("ACCESS DENIED")));
		DisplayText->SetTextRenderColor(FColor::Red);
	}

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [this]()
	{
		if (CurrentState == EPuzzleState::Failed)
		{
			ResetPuzzle();
		}
	}, 2.0f, false);
}

void APuzzle_Keypad::OnReset_Implementation()
{
	ClearInput();
	UpdateDisplay();
}

void APuzzle_Keypad::UpdateDisplay()
{
	if (DisplayText)
	{
		FString DisplayString = CurrentInput.IsEmpty() ? TEXT("____") : CurrentInput;
		DisplayText->SetText(FText::FromString(DisplayString));
		DisplayText->SetTextRenderColor(FColor::White);
	}
}

void APuzzle_Keypad::PlaySound(TObjectPtr<USoundBase> Sound)
{
	if (Sound && AudioComponent)
	{
		AudioComponent->SetSound(Sound);
		AudioComponent->Play();
	}
}