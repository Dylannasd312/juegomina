#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MinaInteractableInterface.h"
#include "ItemBase.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UWidgetComponent;
class AMinaCharacter;

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Key,
	Battery,
	Health,
	Stamina,
	Flashlight,
	Document,
	QuestItem,
	Ammo,
	Custom
};

UCLASS(Blueprintable, BlueprintType)
class JUEGO_MINA_API AItemBase : public AActor, public IMinaInteractableInterface
{
	GENERATED_BODY()

public:
	AItemBase();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Item")
	virtual void Pickup(AMinaCharacter* Character);

	UFUNCTION(BlueprintCallable, Category = "Item")
	virtual void Use(AMinaCharacter* Character);

	UFUNCTION(BlueprintCallable, Category = "Item")
	virtual void Drop(AMinaCharacter* Character);

	UFUNCTION(BlueprintPure, Category = "Item")
	EItemType GetItemType() const { return ItemType; }

	UFUNCTION(BlueprintPure, Category = "Item")
	FText GetItemName() const { return ItemName; }

	UFUNCTION(BlueprintPure, Category = "Item")
	FText GetItemDescription() const { return ItemDescription; }

	UFUNCTION(BlueprintPure, Category = "Item")
	int32 GetQuantity() const { return Quantity; }

	UFUNCTION(BlueprintCallable, Category = "Item")
	void SetQuantity(int32 NewQuantity);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> InteractionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UWidgetComponent> InteractionWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EItemType ItemType = EItemType::Custom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Quantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 MaxStackSize = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bAutoPickup = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bDestroyOnPickup = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Visual")
	TObjectPtr<UMaterialInterface> HighlightMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Audio")
	TObjectPtr<USoundBase> PickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Audio")
	TObjectPtr<USoundBase> UseSound;

private:
	UMaterialInterface* OriginalMaterial = nullptr;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void OnInteract_Implementation(AMinaCharacter* Interactor) override;
	virtual FText GetInteractionText_Implementation() const override;
	virtual bool CanInteract_Implementation(AMinaCharacter* Interactor) const override;
	virtual void OnFocusGained_Implementation(AMinaCharacter* Interactor) override;
	virtual void OnFocusLost_Implementation(AMinaCharacter* Interactor) override;

	void Highlight(bool bEnable);
	void PlaySound(TObjectPtr<USoundBase> Sound);
};