#include "Items/Item_Note.h"
#include "Character/MinaCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

AItem_Note::AItem_Note()
{
	ItemType = EItemType::Document;
	MaxStackSize = 1;
}

void AItem_Note::Use(AMinaCharacter* Character)
{
	Super::Use(Character);
	ShowNote(Character);
}

void AItem_Note::ShowNote(AMinaCharacter* Character)
{
	if (!Character || !NoteWidgetClass)
	{
		return;
	}

	if (!ActiveNoteWidget)
	{
		ActiveNoteWidget = CreateWidget<UUserWidget>(Character->GetWorld(), NoteWidgetClass);
	}

	if (ActiveNoteWidget)
	{
		ActiveNoteWidget->AddToViewport();
		
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(ActiveNoteWidget->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		Character->GetController<APlayerController>()->SetInputMode(InputMode);
		Character->GetController<APlayerController>()->bShowMouseCursor = true;
	}
}

void AItem_Note::HideNote()
{
	if (ActiveNoteWidget)
	{
		ActiveNoteWidget->RemoveFromParent();
		ActiveNoteWidget = nullptr;
	}
}