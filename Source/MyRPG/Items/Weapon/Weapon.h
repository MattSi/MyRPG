// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class UBoxComponent;
class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class MYRPG_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();

	void DeactivateEmbers() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName WeaponName;
	// ===================== Equip & UnEquip =================================
	void Equip(USceneComponent* InParent, FName InSocket, AActor* NewOwner, APawn* NewInstigator);
	void UnEquip();
	void PlayEquipSound();
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);
	// ===================== Equip & UnEquip =================================

	void DisableSphereCollision();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<AActor*> IgnoreActors;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;
	
	// ====================== Defense Action =================================
	UPROPERTY(EditAnywhere, Category = "Defense")
	bool bUseDefenseAction = false;
	UPROPERTY(EditAnywhere, Category="Defence", meta=(EditCondition = "bUseDefence"))
	UAnimMontage* DefenseMontage;
	UPROPERTY(EditAnywhere, Category="Defence", meta=(EditCondition = "bUseDefence"))
	UAnimMontage* DefenseHitMontage;
	UPROPERTY(EditAnywhere, Category="Defence", meta=(EditCondition = "bUseDefence"))
	UNiagaraSystem* DefenseHitNiagara;
	UPROPERTY(EditAnywhere, Category="Defence", meta=(EditCondition = "bUseDefence"))
	float MaxReboundTime = 0.1f;
	UPROPERTY(EditAnywhere, Category="Defence", meta=(EditCondition = "bUseDefence"))
	float ReboundTime = 0.05f;

	void ResetRebound();
	UFUNCTION(BlueprintCallable, Category="Defence")
	virtual void BeginDefense();
	UFUNCTION(BlueprintCallable, Category="Defence")
	virtual void EndDefense();
	// ====================== Defense Action =================================

	// ====================== Attack Action ==================================
	void Attack();
	void AttackEnd();
	UPROPERTY(EditAnywhere, Category="Attack")
	TSubclassOf<UCameraShakeBase> AttackCameraShake;
	UPROPERTY(EditAnywhere, Category="Attack")
	UAnimMontage* AttackMontage;
	int32 CurrentAttackSectionNumber;
	bool bWantContinueAttack;
	// ====================== Attack Action ==================================

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                  const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	USoundBase* EquipSound;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent* WeaponBox;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USceneComponent* BoxTraceEnd;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage = 20.f;

public:
	FORCEINLINE UBoxComponent* GetWeaponBox() { return WeaponBox; }
};
