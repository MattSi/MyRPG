// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyRPGCharacter.h"

#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "KismetAnimationLibrary.h"
#include "Animation/AnimLayerInterface.h"
#include "Components/Grapple/GrappleComponent.h"
#include "DataTable/DataTableManager.h"
#include "Items/Weapon/Weapon.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AMyRPGCharacter

AMyRPGCharacter::AMyRPGCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)


	bShouldMoveEightDirection = false;
	bUseUpperBody = false;

	// Find Weapon Datatable
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableObj(TEXT("/Game/DataTable/DT_Weapon.DT_Weapon"));
	if (DataTableObj.Succeeded())
	{
		WeaponDataTable = DataTableObj.Object;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find DataTable"));
	}

	// Grapple ==
	GrappleComponent = CreateDefaultSubobject<UGrappleComponent>(TEXT("GrappleComponent"));
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMyRPGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyRPGCharacter::Move);

		// Looking
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyRPGCharacter::Look);

		// Change Movement Method
		EnhancedInput->BindAction(ChangeMovementAction, ETriggerEvent::Started, this, &AMyRPGCharacter::ChangeMovement);

		// Attacking
		EnhancedInput->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AMyRPGCharacter::Attack);

		EnhancedInput->BindAction(PickupAction, ETriggerEvent::Started, this, &AMyRPGCharacter::PickupWeapon);
		EnhancedInput->BindAction(DodgeAction, ETriggerEvent::Started, this, &AMyRPGCharacter::Dodge);
		EnhancedInput->BindAction(GrappleAction, ETriggerEvent::Triggered, this, &AMyRPGCharacter::Grapple);

		// Defense
		EnhancedInput->BindAction(DefenseAction, ETriggerEvent::Started, this, &AMyRPGCharacter::Defense);
		EnhancedInput->BindAction(DefenseAction, ETriggerEvent::Completed, this, &AMyRPGCharacter::StopDefense);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ), *GetNameSafe(this));
	}
}

void AMyRPGCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void AMyRPGCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AMyRPGCharacter::EquipWeapon(AWeapon* Weapon)
{
	if (EquippedWeapon)
	{
		// Try to unequip weapon
		EquippedWeapon->UnEquip();
		EquippedWeapon = nullptr;
	}
	Weapon->Equip(GetMesh(), FName("WeaponCarrier"), this, this);
	CharacterState = ECharacterState::ECS_Unequipped;
	OverlappingItem = nullptr;
	EquippedWeapon = Weapon;
}

bool AMyRPGCharacter::CanDisArm()
{
	return ActionState == EActionState::EAS_Idle &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

void AMyRPGCharacter::DisArm()
{
	//PlayEquipMontage(FName("Unequip"));
	if (EquippedWeapon)
	{
		EquippedWeapon->Equip(GetMesh(), FName("WeaponCarrier"), this, this);
	}

	// UDataTable* WeaponDataTable = UDataTableManager::Get()->GetWeaponDataTable();
	FWeaponDataStruct* WeaponRow = WeaponDataTable->FindRow<FWeaponDataStruct>(FName(TEXT("NoWeapon")), TEXT(""));
	if (!WeaponRow) return;
	TSubclassOf<UAnimInstance> AnimLayerClass = WeaponRow->AnimLayerClass;
	SwitchAnimationLayer(AnimLayerClass);
	CharacterState = ECharacterState::ECS_Unequipped;
}

bool AMyRPGCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Idle &&
		CharacterState == ECharacterState::ECS_Unequipped &&
		EquippedWeapon;
}

void AMyRPGCharacter::Arm()
{
	// PlayEquipMontage(FName("Equip"));
	if (EquippedWeapon)
	{
		EquippedWeapon->Equip(GetMesh(), FName("WeaponRightHand"), this, this);
	}

	//UDataTable* WeaponDataTable = UDataTableManager::Get()->GetWeaponDataTable();
	FWeaponDataStruct* WeaponRow = WeaponDataTable->FindRow<FWeaponDataStruct>(EquippedWeapon->WeaponName, TEXT(""));
	if (!WeaponRow) return;
	TSubclassOf<UAnimInstance> AnimLayerClass = WeaponRow->AnimLayerClass;
	SwitchAnimationLayer(AnimLayerClass);
	CharacterState = ECharacterState::ECS_EquippedKatanaWeapon;
}

void AMyRPGCharacter::SwitchAnimationLayer(TSubclassOf<UAnimInstance> AnimLayerClass)
{
	if (USkeletalMeshComponent* SkeletalMeshComponent = GetMesh())
	{
		UE_LOG(LogTemp, Warning, TEXT("SkeletalMeshComponent got."))
		if (UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance())
		{
			UE_LOG(LogTemp, Warning, TEXT("AnimInstance got."))
			if (AnimLayerClass && AnimLayerClass->ImplementsInterface(UAnimLayerInterface::StaticClass()))
			{
				UE_LOG(LogTemp, Warning, TEXT("AnimLayerClass got."))
				AnimInstance->LinkAnimClassLayers(AnimLayerClass);
				UE_LOG(LogTemp, Warning, TEXT("Anim Class layer linked."))
			}
		}
	}
}

void AMyRPGCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquippedWeapon && EquippedWeapon->EquipMontage)
	{
		AnimInstance->Montage_Play(EquippedWeapon->EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquippedWeapon->EquipMontage);
	}
}


// ============================ State machine functions ============================================

bool AMyRPGCharacter::CanMove() const
{
	if (ActionState == EActionState::EAS_Idle) return true;
	return false;
}

bool AMyRPGCharacter::CanAttack() const
{
	return CharacterState != ECharacterState::ECS_Unequipped &&
		ActionState != EActionState::EAS_Dodge &&
		ActionState != EActionState::EAS_EquippingWeapon &&
		ActionState != EActionState::EAS_HitReaction &&
		ActionState != EActionState::EAS_SP_Attacking &&
		ActionState != EActionState::EAS_Grapple;
}

bool AMyRPGCharacter::CanDodge() const
{
	return
		ActionState != EActionState::EAS_Dodge &&
		ActionState != EActionState::EAS_EquippingWeapon &&
		ActionState != EActionState::EAS_HitReaction &&
		ActionState != EActionState::EAS_Grapple;
}

bool AMyRPGCharacter::CanDefense() const
{
	return CharacterState != ECharacterState::ECS_Unequipped &&
		ActionState != EActionState::EAS_Defence &&
		ActionState != EActionState::EAS_EquippingWeapon &&
		ActionState != EActionState::EAS_HitReaction &&
		ActionState != EActionState::EAS_Grapple;
}

bool AMyRPGCharacter::ChangeActionState(EActionState NextState)
{
	ActionState = NextState;
	return true;
}

bool AMyRPGCharacter::CanGrapple() const
{
	return ActionState == EActionState::EAS_Idle;
}

// ============================ State machine functions ========================================================


// ============================ Action Montages Function Begin  ================================================
void AMyRPGCharacter::Dodge(const FInputActionValue& Value)
{
	if (!CanDodge()) return;
	if (!DodgeMontage) return;

	FVector2D Input = Value.Get<FVector2D>();
	FVector InputDirection = FVector(Input.X, Input.Y, 0.0f);
	InputDirection.Normalize();
	FVector WorldVelocity = GetVelocity();
	FRotator WorldRotation = GetActorRotation();
	float Direction = UKismetAnimationLibrary::CalculateDirection(WorldVelocity, GetActorRotation());
	UE_LOG(LogTemp, Warning, TEXT("Direction: %f"), Direction);
	int SectionNumber = 0;
	if (Direction > -45.0f && Direction <= 45.0f) // Forward
	{
		SectionNumber = 0;
	}
	else if (Direction > 45.0f && Direction <= 135.0f) // Right
	{
		SectionNumber = 3;
	}
	else if (Direction > 135.0f || Direction <= -135.0f) // Backward
	{
		SectionNumber = 1;
	}
	else if (Direction > -135.0f && Direction <= -45.0f) // Left
	{
		SectionNumber = 2;
	}
	ChangeActionState(EActionState::EAS_Dodge);
	PlayAnimMontage(DodgeMontage, 1.0f, DodgeMontage->GetSectionName(SectionNumber));
}

void AMyRPGCharacter::DodgeMontageEnded()
{
	ChangeActionState(EActionState::EAS_Idle);
}

void AMyRPGCharacter::AttackEnd()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttackEnd();
	}
}


void AMyRPGCharacter::Attack(const FInputActionValue& Value)
{
	if (!CanAttack() || !EquippedWeapon) return;

	EquippedWeapon->Attack();
}

void AMyRPGCharacter::PickupWeapon(const FInputActionValue& Value)
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);

	if (OverlappingWeapon)
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->Destroy();
		}
		EquipWeapon(OverlappingWeapon);
	}
	else
	{
		/** Don't play arm and disarm anim montage */
		if (CanDisArm())
			DisArm();
		else if (CanArm())
			Arm();
	}
}

void AMyRPGCharacter::Grapple(const FInputActionValue& Value)
{
	if (!CanGrapple()) return;

	ChangeActionState(EActionState::EAS_Grapple);

	GrappleComponent->EquipHook();
}

bool AMyRPGCharacter::CanGrapple()
{
	return ActionState == EActionState::EAS_Idle;
}


void AMyRPGCharacter::Defense(const FInputActionValue& Value)
{
	if(DefenseMontage)
	{
		PlayAnimMontage(DefenseMontage);
	}
}

void AMyRPGCharacter::StopDefense(const FInputActionValue& Value)
{
}
// ============================ Action Montages Function End    ================================================



void AMyRPGCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}


void AMyRPGCharacter::Move(const FInputActionValue& Value)
{
	if (!CanMove()) return;

	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (!Controller) return;
	if (GetCurrentActionState() == EActionState::EAS_Idle)
	{
		StopAnimMontage();
	}


	// find out which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	// get right vector 
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// add movement 
	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AMyRPGCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMyRPGCharacter::ChangeMovement(const FInputActionValue& Value)
{
	bShouldMoveEightDirection = !bShouldMoveEightDirection;
}
