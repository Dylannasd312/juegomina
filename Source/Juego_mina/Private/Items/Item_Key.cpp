#include "Items/Item_Key.h"
#include "Character/MinaCharacter.h"
#include "GameplayTagsModule.h"

AItem_Key::AItem_Key()
{
	ItemType = EItemType::Key;
	MaxStackSize = 1;
}

void AItem_Key::BeginPlay()
{
	Super::BeginPlay();

	if (!KeyTag.IsValid())
	{
		KeyTag = FGameplayTag::RequestGameplayTag(FName(*FString::Printf(TEXT("Key.%s"), *GetName())));
	}
}

void AItem_Key::Pickup(AMinaCharacter* Character)
{
	Super::Pickup(Character);

	if (Character && KeyTag.IsValid())
	{
		Character->AddGameplayTag(KeyTag);
	}
}