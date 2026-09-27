#include "Character/MinaCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Components/SpotLightComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

AMinaCharacter::AMinaCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = DefaultCameraDistance;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = CameraLagSpeed;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
	Flashlight->SetupAttachment(Camera);
	Flashlight->SetRelativeLocation(FVector(20.0f, 10.0f, 10.0f));
	Flashlight->Intensity = 5000.0f;
	Flashlight->OuterConeAngle = 45.0f;
	Flashlight->InnerConeAngle = 30.0f;
	Flashlight->SetVisibility(false);

	CurrentHealth = MaxHealth;
	CurrentStamina = MaxStamina;
}

void AMinaCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}

void AMinaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction) EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMinaCharacter::Move);
		if (LookAction) EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMinaCharacter::Look);
		if (JumpAction) EnhancedInput->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AMinaCharacter::OnJump);
		if (SprintAction)
		{
			EnhancedInput->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AMinaCharacter::StartSprint);
			EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMinaCharacter::StopSprint);
		}
		if (CrouchAction)
		{
			EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AMinaCharacter::StartCrouch);
			EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AMinaCharacter::StopCrouch);
		}
		if (InteractAction) EnhancedInput->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AMinaCharacter::OnInteract);
		if (FlashlightAction) EnhancedInput->BindAction(FlashlightAction, ETriggerEvent::Triggered, this, &AMinaCharacter::OnToggleFlashlight);
	}
}

void AMinaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateMovementState();
	UpdateStamina(DeltaTime);
	HandleFootsteps(DeltaTime);
}

void AMinaCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller && (MovementVector.X != 0.0f || MovementVector.Y != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMinaCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller && (LookAxisVector.X != 0.0f || LookAxisVector.Y != 0.0f))
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMinaCharacter::StartSprint(const FInputActionValue& Value)
{
	bWantsToSprint = true;
}

void AMinaCharacter::StopSprint(const FInputActionValue& Value)
{
	bWantsToSprint = false;
}

void AMinaCharacter::StartCrouch(const FInputActionValue& Value)
{
	Crouch();
}

void AMinaCharacter::StopCrouch(const FInputActionValue& Value)
{
	UnCrouch();
}

void AMinaCharacter::OnJump(const FInputActionValue& Value)
{
	Jump();
}

void AMinaCharacter::OnInteract(const FInputActionValue& Value)
{
	Interact();
}

void AMinaCharacter::OnToggleFlashlight(const FInputActionValue& Value)
{
	ToggleFlashlight();
}

void AMinaCharacter::UpdateMovementState()
{
	EMovementState NewState = EMovementState::Idle;

	if (GetCharacterMovement()->IsFalling())
	{
		NewState = EMovementState::Falling;
	}
	else if (GetVelocity().Size() > 1.0f)
	{
		if (bIsSprinting && CurrentStamina > 0.0f)
		{
			NewState = EMovementState::Running;
		}
		else if (bIsCrouched)
		{
			NewState = EMovementState::Crouching;
		}
		else
		{
			NewState = EMovementState::Walking;
		}
	}

	if (NewState != CurrentMovementState)
	{
		SetMovementState(NewState);
	}

	bool bShouldSprint = bWantsToSprint && !bIsCrouched && CurrentStamina > 0.0f && GetVelocity().Size() > 10.0f;
	if (bShouldSprint != bIsSprinting)
	{
		bIsSprinting = bShouldSprint;
		GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? RunSpeed : (bIsCrouched ? CrouchSpeed : WalkSpeed);
	}
}

void AMinaCharacter::UpdateStamina(float DeltaTime)
{
	if (bIsSprinting)
	{
		CurrentStamina = FMath::Max(0.0f, CurrentStamina - StaminaDrainRate * DeltaTime);
	}
	else if (CurrentStamina < MaxStamina)
	{
		CurrentStamina = FMath::Min(MaxStamina, CurrentStamina + StaminaRegenRate * DeltaTime);
	}
}

void AMinaCharacter::HandleFootsteps(float DeltaTime)
{
	if (GetVelocity().Size() < 10.0f || GetCharacterMovement()->IsFalling())
	{
		FootstepTimer = 0.0f;
		return;
	}

	float SpeedMultiplier = bIsSprinting ? 0.6f : 1.0f;
	FootstepTimer += DeltaTime * SpeedMultiplier;

	if (FootstepTimer >= FootstepInterval)
	{
		ESurfaceType Surface = GetSurfaceType();
		PlayFootstepSound(Surface);
		FootstepTimer = 0.0f;
	}
}

ESurfaceType AMinaCharacter::GetSurfaceType() const
{
	FHitResult HitResult;
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0.0f, 0.0f, 100.0f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
	{
		if (UPhysicalMaterial* PhysMat = HitResult.PhysMaterial.Get())
		{
			FName SurfaceName = PhysMat->SurfaceType;

			if (SurfaceName == UPhysicalMaterial::GetSurfaceTypeName(ESurfaceType::Surface_Wood)) return ESurfaceType::Surface_Wood;
			if (SurfaceName == UPhysicalMaterial::GetSurfaceTypeName(ESurfaceType::Surface_Metal)) return ESurfaceType::Surface_Metal;
			if (SurfaceName == UPhysicalMaterial::GetSurfaceTypeName(ESurfaceType::Surface_Stone)) return ESurfaceType::Surface_Stone;
			if (SurfaceName == UPhysicalMaterial::GetSurfaceTypeName(ESurfaceType::Surface_Grass)) return ESurfaceType::Surface_Grass;
			if (SurfaceName == UPhysicalMaterial::GetSurfaceTypeName(ESurfaceType::Surface_Dirt)) return ESurfaceType::Surface_Dirt;
			if (SurfaceName == UPhysicalMaterial::GetSurfaceTypeName(ESurfaceType::Surface_Carpet)) return ESurfaceType::Surface_Carpet;
			if (SurfaceName == UPhysicalMaterial::GetSurfaceTypeName(ESurfaceType::Surface_Tile)) return ESurfaceType::Surface_Tile;
			if (SurfaceName == UPhysicalMaterial::GetSurfaceTypeName(ESurfaceType::Surface_Water)) return ESurfaceType::Surface_Water;
		}
	}

	return ESurfaceType::Surface_Default;
}

void AMinaCharacter::SetMovementState(EMovementState NewState)
{
	CurrentMovementState = NewState;
}

void AMinaCharacter::SetCameraMode(bool bFirstPerson)
{
	bFirstPersonMode = bFirstPerson;
	float TargetDistance = bFirstPerson ? FirstPersonCameraDistance : DefaultCameraDistance;

	if (SpringArm)
	{
		SpringArm->TargetArmLength = TargetDistance;
		SpringArm->bUsePawnControlRotation = !bFirstPerson;
	}

	if (Camera)
	{
		Camera->bUsePawnControlRotation = bFirstPerson;
	}
}

void AMinaCharacter::SetCameraDistance(float Distance)
{
	if (SpringArm && !bFirstPersonMode)
	{
		SpringArm->TargetArmLength = FMath::Clamp(Distance, 100.0f, 500.0f);
	}
}

void AMinaCharacter::Interact()
{
	FHitResult HitResult;
	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + Camera->GetForwardVector() * 300.0f;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel1, Params))
	{
		if (AActor* HitActor = HitResult.GetActor())
		{
			HitActor->ReceiveMessage(TEXT("OnInteract"), this, true);
		}
	}
}

void AMinaCharacter::ToggleFlashlight()
{
	if (Flashlight)
	{
		Flashlight->SetVisibility(!Flashlight->IsVisible());
	}
}

void AMinaCharacter::PlayFootstepSound_Implementation(ESurfaceType SurfaceType)
{
	FFootstepData* FootstepData = SurfaceFootsteps.Find(SurfaceType);
	if (!FootstepData)
	{
		FootstepData = &DefaultFootstep;
	}

	if (FootstepData->Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FootstepData->Sound, GetActorLocation(), FootstepData->Volume, FootstepData->Pitch);
	}
}

void AMinaCharacter::TakeDamage_Implementation(float DamageAmount, AActor* DamageCauser)
{
	CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);

	if (CurrentHealth <= 0.0f)
	{
		OnDeath();
	}
}

void AMinaCharacter::Heal_Implementation(float HealAmount)
{
	CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + HealAmount);
}

void AMinaCharacter::OnDeath()
{
}