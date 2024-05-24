// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon/Weapon.h"

#include "MyRPGCharacter.h"
#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Interfaces/HitInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"


AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::DeactivateEmbers() const
{
	if (ItemEffect)
	{
		ItemEffect->Deactivate();
	}
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                           int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	IgnoreActors.AddUnique(this);
	IgnoreActors.AddUnique(GetOwner());
	FHitResult BoxHit;
	UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		FVector(5.f, 5.f, 5.f),
		BoxTraceStart->GetComponentRotation(),
		TraceTypeQuery1,
		false,
		IgnoreActors,
		EDrawDebugTrace::None,
		BoxHit,
		true);

	if (BoxHit.GetActor())
	{
		UGameplayStatics::ApplyDamage(
			BoxHit.GetActor(),
			Damage,
			GetInstigator()->GetController(),
			this,
			UDamageType::StaticClass());

		IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
		if (HitInterface)
		{
			HitInterface->GetHit(BoxHit.ImpactPoint, GetOwner());
		}
		IgnoreActors.AddUnique(BoxHit.GetActor());
	}
}

// ==================================== Equip & UnEquip =======================================================
void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AWeapon::DisableSphereCollision()
{
	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocket, AActor* NewOwner, APawn* NewInstigator)
{
	ItemState = EItemState::EIS_Equipped;
	AttachMeshToSocket(InParent, InSocket);
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	DisableSphereCollision();
	DeactivateEmbers();
}

void AWeapon::UnEquip()
{
	//
	// AttachMeshToSocket(InPar)
}

void AWeapon::PlayEquipSound()
{
}

// ==================================== Equip & UnEquip =======================================================

// ====================== Defense Action =================================
void AWeapon::ResetRebound()
{
}

void AWeapon::BeginDefense()
{
}

void AWeapon::EndDefense()
{
}


// ====================== Defense Action =================================


// ====================== Attack Action ==================================

void AWeapon::Attack()
{
	if (!AttackMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Attack Montage."))
		return;
	}

	AMyRPGCharacter* OwnerPlayer = Cast<AMyRPGCharacter>(GetOwner());
	if (!OwnerPlayer)
	{
		return;
	}

	if (OwnerPlayer->ActionState != EActionState::EAS_Attacking)
	{
		CurrentAttackSectionNumber = 0;
		OwnerPlayer->PlayAnimMontage(AttackMontage, 1.0f, AttackMontage->GetSectionName(CurrentAttackSectionNumber));
		OwnerPlayer->ChangeActionState(EActionState::EAS_Attacking);
	}
	else
	{
		if (CurrentAttackSectionNumber < AttackMontage->GetNumSections() - 1)
		{
			bWantContinueAttack = true;
		}
	}
}

void AWeapon::AttackEnd()
{
	AMyRPGCharacter* OwnerPlayer = Cast<AMyRPGCharacter>(GetOwner());
	if (!OwnerPlayer)
		return;

	if (bWantContinueAttack)
	{
		bWantContinueAttack = false;
		if (CurrentAttackSectionNumber == AttackMontage->GetNumSections() - 1)
			return;
		CurrentAttackSectionNumber += 1;
		OwnerPlayer->PlayAnimMontage(AttackMontage, 1.0f, AttackMontage->GetSectionName(CurrentAttackSectionNumber));
	}
	else
	{
		OwnerPlayer->ChangeActionState(EActionState::EAS_Idle);
		bWantContinueAttack = false;
	}
}

// ====================== Attack Action ==================================
