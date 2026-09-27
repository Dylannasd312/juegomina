#pragma once

#include "CoreMinimal.h"
#include "Items/ItemBase.h"
#include "Item_Note.generated.h"

class AMinaCharacter;
class UUserWidget;

UCLASS(Blueprintable, BlueprintType)
class JUEGO_MINA_API AItem_Note : public AItemBase
{
	GENERATED_BODY()

public:
	AItem_Note();

public:
	UFUNCTION(BlueprintCallable, Category = "Note")
	virtual void Use(AMinaCharacter* Character) override;

	UFUNCTION(BlueprintCallable, Category = "Note")
	void ShowNote(AMinaCharacter* Character);

	UFUNCTION(BlueprintCallable, Category = "Note")
	void HideNote();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note")
	FText NoteTitle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note", meta = (MultiLine = true))
	FText NoteContent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note")
	TSubclassOf<UUserWidget> NoteWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note")
	bool bAutoShowOnPickup = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note")
	FGameplayTag QuestTag;

private:
	TObjectPtr<UUserWidget> ActiveNoteWidget = nullptr;
};