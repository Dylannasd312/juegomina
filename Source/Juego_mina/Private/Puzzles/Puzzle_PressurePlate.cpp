#include "Puzzles/Puzzle_PressurePlate.h"
#include "Character/MinaCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameplayTagsModule.h"
#include "Kismet/GameplayStatics.h"

APuzzle_PressurePlate::APuzzle_PressurePlate()
{
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetupAttachment(RootComponent);
	TriggerVolume->SetBoxExtent(FVector(100.0f, 100.0f, 50.0f));
	TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerVolume->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APuzzle_PressurePlate::OnOverlapBegin);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &APuzzle_PressurePlate::OnOverlapEnd);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);
	AudioComponent->bAutoActivate = false;
}

void APuzzle_PressurePlate::BeginPlay()
{
	Super::BeginPlay();
	UpdateVisuals();
}

void APuzzle_PressurePlate::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	if (RequiredActorTag.IsValid() && !OtherActor->ActorHasTag(RequiredActorTag.GetTagName()))
	{
		return;
	}

	OverlappingActors.AddUnique(OtherActor);
	CheckActivation();
}

void APuzzle_PressurePlate::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OverlappingActors.Remove(OtherActor);
	CheckActivation();
}

void APuzzle_PressurePlate::CheckActivation()
{
	float TotalWeight = 0.0f;
	bool bHasRequiredTag = !RequiredActorTag.IsValid();

	for (AActor* Actor : OverlappingActors)
	{
		if (!Actor)
		{
			continue;
		}

		if (UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(Actor->GetRootComponent()))
		{
			TotalWeight += RootComp->GetMass();
		}

		if (RequiredActorTag.IsValid() && Actor->ActorHasTag(RequiredActorTag.GetTagName()))
		{
			bHasRequiredTag = true;
		}
	}

	bool bShouldActivate = (TotalWeight >= RequiredWeight) && bHasRequiredTag;

	if (bShouldActivate && !bIsActivated)
	{
		bIsActivated = true;
		ActivatePuzzle();
		SolvePuzzle();
		PlayPlateSound(ActivateSound);
		UpdateVisuals();
	}
	else if (!bShouldActivate && bIsActivated && bRequireContinuousPressure)
	{
		bIsActivated = false;
		DeactivatePuzzle();
		PlayPlateSound(DeactivateSound);
		UpdateVisuals();
	}
}

void APuzzle_PressurePlate::OnActivate_Implementation()
{
	bIsActivated = true;
	UpdateVisuals();
}

void APuzzle_PressurePlate::OnDeactivate_Implementation()
{
	bIsActivated = false;
	UpdateVisuals();
}

void APuzzle_PressurePlate::SetRequiredWeight(float Weight)
{
	RequiredWeight = Weight;
}

void APuzzle_PressurePlate::SetRequiredTag(FGameplayTag Tag)
{
	RequiredActorTag = Tag;
}

void APuzzle_PressurePlate::UpdateVisuals()
{
	if (MeshComponent)
	{
		MeshComponent->SetMaterial(0, bIsActivated ? ActiveMaterial : InactiveMaterial);
	}
}

void APuzzle_PressurePlate::PlayPlateSound(TObjectPtr<USoundBase> Sound)
{
	if (Sound && AudioComponent)
	{
		AudioComponent->SetSound(Sound);
		AudioComponent->Play();
	}
}