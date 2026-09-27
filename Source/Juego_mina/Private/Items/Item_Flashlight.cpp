#include "Items/Item_Flashlight.h"
#include "Character/MinaCharacter.h"

AItem_Flashlight::AItem_Flashlight()
{
	ItemType = EItemType::Flashlight;
	MaxStackSize = 1;
}

void AItem_Flashlight::Pickup(AMinaCharacter* Character)
{
	Super::Pickup(Character);

	if (Character)
	{
		Character->ToggleFlashlight();
	}
}

void AItem_Flashlight::Use(AMinaCharacter* Character)
{
	Super::Use(Character);

	if (Character)
	{
		Character->ToggleFlashlight();
	}
}