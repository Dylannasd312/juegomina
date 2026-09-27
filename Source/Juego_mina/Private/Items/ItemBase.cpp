#include "Items/ItemBase.h"
#include "Character/MinaCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

AItemBase::AItemBase()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->SetSphereRadius(150.0f);
	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &AItemBase::OnOverlapBegin);
	InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &AItemBase::OnOverlapEnd);

	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(RootComponent);
	InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidget->SetDrawSize(FVector2D(200.0f, 50.0f));
	InteractionWidget->SetVisibility(false);
}

void AItemBase::BeginPlay()
{
	Super::BeginPlay();

	OriginalMaterial = MeshComponent->GetMaterial(0);
}

void AItemBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bAutoPickup)
	{
		if (AMinaCharacter* Character = Cast<AMinaCharacter>(OtherActor))
		{
			Pickup(Character);
		}
	}
}

void AItemBase::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AItemBase::OnInteract_Implementation(AMinaCharacter* Interactor)
{
	if (Interactor && CanInteract(Interactor))
	{
		Pickup(Interactor);
	}
}

FText AItemBase::GetInteractionText_Implementation() const
{
	return FText::Format(FText::FromString(TEXT("Recoger {0}")), ItemName);
}

bool AItemBase::CanInteract_Implementation(AMinaCharacter* Interactor) const
{
	return Interactor != nullptr;
}

void AItemBase::OnFocusGained_Implementation(AMinaCharacter* Interactor)
{
	Highlight(true);
	InteractionWidget->SetVisibility(true);
}

void AItemBase::OnFocusLost_Implementation(AMinaCharacter* Interactor)
{
	Highlight(false);
	InteractionWidget->SetVisibility(false);
}

void AItemBase::Pickup(AMinaCharacter* Character)
{
	if (!Character)
	{
		return;
	}

	PlaySound(PickupSound);

	if (bDestroyOnPickup)
	{
		Destroy();
	}
	else
	{
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		SetActorTickEnabled(false);
	}
}

void AItemBase::Use(AMinaCharacter* Character)
{
	PlaySound(UseSound);
}

void AItemBase::Drop(AMinaCharacter* Character)
{
	if (!Character)
	{
		return;
	}

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	SetActorLocation(Character->GetActorLocation() + Character->GetActorForwardVector() * 100.0f + FVector(0.0f, 0.0f, 50.0f));
}

void AItemBase::SetQuantity(int32 NewQuantity)
{
	Quantity = FMath::Clamp(NewQuantity, 0, MaxStackSize);
}

void AItemBase::Highlight(bool bEnable)
{
	if (MeshComponent)
	{
		if (bEnable && HighlightMaterial)
		{
			MeshComponent->SetMaterial(0, HighlightMaterial);
		}
		else
		{
			MeshComponent->SetMaterial(0, OriginalMaterial);
		}
	}
}

void AItemBase::PlaySound(TObjectPtr<USoundBase> Sound)
{
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sound, GetActorLocation());
	}
}