#include "Items/Item_Battery.h"
#include "Character/MinaCharacter.h"

AItem_Battery::AItem_Battery()
{
	ItemType = EItemType::Battery;
	MaxStackSize = 5;
}

void AItem_Battery::Use(AMinaCharacter* Character)
{
	Super::Use(Character);

	if (!Character)
	{
		return;
	}

	if (bRestoreStamina)
	{
		Character->Heal(StaminaRestoreAmount);
	}

	Quantity--;
	if (Quantity <= 0)
	{
		Destroy();
	}
}