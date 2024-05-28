// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterTypes.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MyRPGCharacter.generated.h"

class UGrappleComponent;
class UCableComponent;
class AEnemyBaseCharacter;
class AWeapon;
class AItem;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AMyRPGCharacter : public ACharacter
{

private:
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Change movement Action (Temp) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ChangeMovementAction;

	/** Attack Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	/** Equip Weapon Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PickupAction;

	/** Dodge Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DodgeAction;

	/** Dodge Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* GrappleAction;

	
	UPROPERTY(VisibleInstanceOnly)
	AItem *OverlappingItem;
public:
	AMyRPGCharacter();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Method")
	bool bShouldMoveEightDirection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	bool bUseUpperBody;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for changing movement method */
	void ChangeMovement(const FInputActionValue& Value);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();


public:
	virtual void Tick(float DeltaSeconds) override;
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE EActionState GetCurrentActionState() const {return ActionState;}

	// ============================ State machine functions ============================================
	UPROPERTY(BlueprintReadWrite)
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	UPROPERTY(BlueprintReadWrite)
	EActionState ActionState = EActionState::EAS_Idle;
	bool CanMove() const;
	bool CanAttack() const;
	bool CanDodge() const;
	bool CanDefense() const;
	bool ChangeActionState(EActionState NextState);
	bool CanGrapple() const;
	// ============================ State machine functions ============================================


	// ============================ Action Dodge ===========================================
	void Dodge(const FInputActionValue& Value);
	UPROPERTY(EditAnywhere, Category="Action")
	UAnimMontage* DodgeMontage;
	FOnMontageEnded OnDodgeMontageEnded;
	UFUNCTION()
	void DodgeMontageEnded();
	// ============================ Action Dodge ===========================================


	// ============================ Action Attack ===========================================
	/** Called for attack method */
	void Attack(const FInputActionValue& Value);
	void AttackEnd();
	// ============================ Action Attack ===========================================

	// ============================ Action Equip, Arm & Unarm ===============================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	AWeapon* EquippedWeapon;
	void EquipWeapon(AWeapon *Weapon);
	bool CanDisArm();
	void DisArm();
	bool CanArm();
	void Arm();
	void PlayEquipMontage(const FName& SectionName);
	/** Called for Equip weapon method */
	void PickupWeapon(const FInputActionValue& Value);
	// ============================ Action Equip, Arm & Unarm ===============================

	// ============================ Action Grapple ==========================================
	void Grapple(const FInputActionValue& Value);
	bool CanGrapple();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Grapple)
	UGrappleComponent* GrappleComponent;
	
	// ============================ Action Grapple ==========================================
	
	// ============================ Lock Enemy ==============================================
	AEnemyBaseCharacter *LockedEnemy;
	bool bIsLockedOnEnabled;
	// ============================ Lock Enemy ==============================================

	
	void SetOverlappingItem(AItem *Item);

	// ============================ Switch Animation Layer ==================================
	void SwitchAnimationLayer(TSubclassOf<UAnimInstance> AnimLayerClass);
	UDataTable* WeaponDataTable;
};


