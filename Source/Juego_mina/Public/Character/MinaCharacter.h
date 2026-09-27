#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "PhysicalMaterial.h"
#include "MinaCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
class UAnimInstance;
class USoundBase;
class UCurveFloat;

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Idle,
	Walking,
	Running,
	Crouching,
	Jumping,
	Falling
};

UENUM(BlueprintType)
enum class ESurfaceType : uint8
{
	Surface_Default,
	Surface_Wood,
	Surface_Metal,
	Surface_Stone,
	Surface_Grass,
	Surface_Dirt,
	Surface_Carpet,
	Surface_Tile,
	Surface_Water
};

USTRUCT(BlueprintType)
struct FFootstepData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footsteps")
	TObjectPtr<USoundBase> Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footsteps")
	float Volume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footsteps")
	float Pitch = 1.0f;
};

USTRUCT(BlueprintType)
struct FSurfaceFootstepData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footsteps")
	TMap<ESurfaceType, FFootstepData> FootstepSounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footsteps")
	FFootstepData DefaultSound;
};

UCLASS(Blueprintable, BlueprintType)
class JUEGO_MINA_API AMinaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMinaCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetMovementState(EMovementState NewState);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	EMovementState GetMovementState() const { return CurrentMovementState; }

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SetCameraMode(bool bFirstPerson);

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SetCameraDistance(float Distance);

	UFUNCTION(BlueprintCallable, Category = "Interact")
	virtual void Interact();

	UFUNCTION(BlueprintCallable, Category = "Flashlight")
	void ToggleFlashlight();

	UFUNCTION(BlueprintNativeEvent, Category = "Footsteps")
	void PlayFootstepSound(ESurfaceType SurfaceType);
	virtual void PlayFootstepSound_Implementation(ESurfaceType SurfaceType);

	UFUNCTION(BlueprintNativeEvent, Category = "Health")
	void TakeDamage(float DamageAmount, AActor* DamageCauser);
	virtual void TakeDamage_Implementation(float DamageAmount, AActor* DamageCauser);

	UFUNCTION(BlueprintNativeEvent, Category = "Health")
	void Heal(float HealAmount);
	virtual void Heal_Implementation(float HealAmount);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flashlight", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpotLightComponent> Flashlight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> FlashlightAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RunSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CrouchSpeed = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float DefaultCameraDistance = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float FirstPersonCameraDistance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraLagSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footsteps")
	float FootstepInterval = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footsteps")
	TMap<ESurfaceType, FFootstepData> SurfaceFootsteps;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footsteps")
	FFootstepData DefaultFootstep;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float CurrentHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float MaxStamina = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina")
	float CurrentStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaDrainRate = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaRegenRate = 15.0f;

private:
	EMovementState CurrentMovementState = EMovementState::Idle;
	bool bIsSprinting = false;
	bool bWantsToSprint = false;
	bool bFirstPersonMode = false;
	float FootstepTimer = 0.0f;
	ESurfaceType LastSurfaceType = ESurfaceType::Surface_Default;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartSprint(const FInputActionValue& Value);
	void StopSprint(const FInputActionValue& Value);
	void StartCrouch(const FInputActionValue& Value);
	void StopCrouch(const FInputActionValue& Value);
	void OnJump(const FInputActionValue& Value);
	void OnInteract(const FInputActionValue& Value);
	void OnToggleFlashlight(const FInputActionValue& Value);

	void UpdateMovementState();
	void UpdateStamina(float DeltaTime);
	void HandleFootsteps(float DeltaTime);
	ESurfaceType GetSurfaceType() const;
};