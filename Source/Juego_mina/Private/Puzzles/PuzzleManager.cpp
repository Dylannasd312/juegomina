#include "Puzzles/PuzzleManager.h"
#include "Puzzles/PuzzleBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

APuzzleManager::APuzzleManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APuzzleManager::BeginPlay()
{
	Super::BeginPlay();
	LoadPuzzleProgress();
}

APuzzleManager* APuzzleManager::GetPuzzleManager(UObject* WorldContextObject)
{
	if (UWorld* World = WorldContextObject->GetWorld())
	{
		for (TActorIterator<APuzzleManager> It(World); It; ++It)
		{
			return *It;
		}
	}
	return nullptr;
}

void APuzzleManager::RegisterPuzzle(APuzzleBase* Puzzle, const FString& PuzzleID)
{
	if (!Puzzle || PuzzleID.IsEmpty())
	{
		return;
	}

	RegisteredPuzzles.Add(PuzzleID, Puzzle);

	if (!PuzzleProgress.Contains(PuzzleID))
	{
		FPuzzleProgress Progress;
		Progress.PuzzleID = PuzzleID;
		Progress.State = EPuzzleState::Inactive;
		Progress.CompletionTime = 0.0f;
		Progress.AttemptCount = 0;
		PuzzleProgress.Add(PuzzleID, Progress);
	}

	Puzzle->OnPuzzleStateChanged.AddDynamic(this, &APuzzleManager::OnPuzzleStateChanged);
	Puzzle->OnPuzzleSolved.AddDynamic(this, &APuzzleManager::OnPuzzleSolvedInternal);
}

void APuzzleManager::UnregisterPuzzle(APuzzleBase* Puzzle)
{
	if (!Puzzle)
	{
		return;
	}

	for (auto It = RegisteredPuzzles.CreateIterator(); It; ++It)
	{
		if (It.Value() == Puzzle)
		{
			Puzzle->OnPuzzleStateChanged.RemoveDynamic(this, &APuzzleManager::OnPuzzleStateChanged);
			Puzzle->OnPuzzleSolved.RemoveDynamic(this, &APuzzleManager::OnPuzzleSolvedInternal);
			It.RemoveCurrent();
			break;
		}
	}
}

void APuzzleManager::OnPuzzleStateChanged(APuzzleBase* Puzzle, EPuzzleState NewState)
{
	if (!Puzzle)
	{
		return;
	}

	FString PuzzleID;
	for (const auto& Pair : RegisteredPuzzles)
	{
		if (Pair.Value == Puzzle)
		{
			PuzzleID = Pair.Key;
			break;
		}
	}

	if (!PuzzleID.IsEmpty() && PuzzleProgress.Contains(PuzzleID))
	{
		FPuzzleProgress& Progress = PuzzleProgress[PuzzleID];
		Progress.State = NewState;

		if (NewState == EPuzzleState::Solved)
		{
			Progress.CompletionTime = GetWorld()->GetTimeSeconds();
		}
		else if (NewState == EPuzzleState::Active)
		{
			Progress.AttemptCount++;
		}
	}

	SavePuzzleProgress();
}

void APuzzleManager::OnPuzzleSolvedInternal(APuzzleBase* Puzzle)
{
	OnPuzzleCompleted.Broadcast(Puzzle);
}

EPuzzleState APuzzleManager::GetPuzzleState(const FString& PuzzleID) const
{
	if (PuzzleProgress.Contains(PuzzleID))
	{
		return PuzzleProgress[PuzzleID].State;
	}
	return EPuzzleState::Inactive;
}

bool APuzzleManager::IsPuzzleSolved(const FString& PuzzleID) const
{
	return GetPuzzleState(PuzzleID) == EPuzzleState::Solved;
}

TArray<FString> APuzzleManager::GetSolvedPuzzles() const
{
	TArray<FString> Solved;
	for (const auto& Pair : PuzzleProgress)
	{
		if (Pair.Value.State == EPuzzleState::Solved)
		{
			Solved.Add(Pair.Key);
		}
	}
	return Solved;
}

TArray<FString> APuzzleManager::GetActivePuzzles() const
{
	TArray<FString> Active;
	for (const auto& Pair : PuzzleProgress)
	{
		if (Pair.Value.State == EPuzzleState::Active)
		{
			Active.Add(Pair.Key);
		}
	}
	return Active;
}

void APuzzleManager::ResetAllPuzzles()
{
	for (const auto& Pair : RegisteredPuzzles)
	{
		if (Pair.Value && Pair.Value->GetPuzzleState() != EPuzzleState::Inactive)
		{
			Pair.Value->ResetPuzzle();
		}
	}

	for (auto& Pair : PuzzleProgress)
	{
		Pair.Value.State = EPuzzleState::Inactive;
		Pair.Value.CompletionTime = 0.0f;
		Pair.Value.AttemptCount = 0;
	}

	SavePuzzleProgress();
}

void APuzzleManager::SavePuzzleProgress()
{
	// Implement save game logic here using USaveGame
	// This is a placeholder for the save system
}

void APuzzleManager::LoadPuzzleProgress()
{
	// Implement load game logic here using USaveGame
	// This is a placeholder for the load system
}