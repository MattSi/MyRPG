// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Grapple/GrappleComponent.h"

#include "CableComponent.h"
#include "GrappleHook.h"
#include "GrappleHookTarget.h"
#include "MyRPGCharacter.h"
#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/PawnMovementComponent.h"

// Sets default values for this component's properties
UGrappleComponent::UGrappleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...


	GrappleType = EGrappleType::EGT_Inactive;
	Timeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Timeline"));
}


// Called when the game starts
void UGrappleComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OwnerCharacter = Cast<AMyRPGCharacter>(GetOwner());
	USkeletalMeshComponent* SkeletalMesh = OwnerCharacter->GetMesh();
	if (CableComponent)
	{
		CableComponent->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::KeepRelativeTransform, FName("WeaponRightHand"));
		CableComponent->SetVisibility(false);
	}

	if (LerpCurve)
	{
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this, FName("TimelineCallback"));
		Timeline->AddInterpFloat(LerpCurve, TimelineCallback);

		FOnTimelineEventStatic TimelineFinishedCallback;
		TimelineFinishedCallback.BindUFunction(this, FName("TimelineFinishedCallback"));
		Timeline->SetTimelineFinishedFunc(TimelineFinishedCallback);
	}
}
// Called every frame
void UGrappleComponent::TickComponent(float DeltaTime, ELevelTick TickType,
									  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (!OwnerCharacter) return;
	switch (GrappleType)
	{
	default:
	case EGrappleType::EGT_Inactive:
		Inactive();
		break;
	case EGrappleType::EGT_Firing:
		Firing();
		break;
	case EGrappleType::EGT_NearTarget:
		NearTarget();
		break;
	case EGrappleType::EGT_Waiting:
		break;
	}
}

void UGrappleComponent::Inactive()
{
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.bReturnPhysicalMaterial = false;
	QueryParams.AddIgnoredActor(OwnerCharacter);

	TArray<FOverlapResult> OverlapResults;
	if (GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		OwnerCharacter->GetActorLocation(),
		FQuat::Identity,
		ECC_WorldStatic,
		FCollisionShape::MakeSphere(MaxGrappleDistance),
		QueryParams
	))
	{
		FindBestTarget(OverlapResults);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Inactive: OverlapMultiByChannel Missed."))
	}
}

void UGrappleComponent::Firing()
{
	if (GrappleHookTarget && GrappleHook)
	{
		float DistanceToTarget = FVector::Distance(GrappleHook->GetActorLocation(),
		                                           GrappleHookTarget->GetActorLocation());
		if (DistanceToTarget < 50.0f) // Set your threshold distance here
		{
			GrappleType = EGrappleType::EGT_NearTarget;
		}
	}
}

void UGrappleComponent::NearTarget()
{
	if (!Timeline->IsPlaying()){
		Timeline->PlayFromStart();
	}
		
}

void UGrappleComponent::FindBestTarget(TArray<FOverlapResult>& OverlapResults)
{
	AGrappleHookTarget* BestActor = nullptr;
	double BestAngle = 180.0;
	for (auto& Result : OverlapResults)
	{
		AGrappleHookTarget* CurrentActor = Cast<AGrappleHookTarget>(Result.GetActor());
		if (!CurrentActor) continue;
		FHitResult HitResult;
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.bTraceComplex = false;
		CollisionQueryParams.AddIgnoredActor(OwnerCharacter);

		if (GetWorld()->LineTraceSingleByChannel(
			HitResult,
			OwnerCharacter->GetActorLocation(),
			CurrentActor->GetActorLocation(),
			ECC_Visibility,
			CollisionQueryParams
		))
		{
			if (HitResult.GetActor() == CurrentActor)
			{
				FVector Direction = CurrentActor->GetActorLocation() - OwnerCharacter->GetActorLocation();
				Direction.Normalize();
				FVector CameraForward = OwnerCharacter->GetFollowCamera()->GetForwardVector();
				const double CurrentAngle = FMath::Acos(FVector::DotProduct(Direction, CameraForward));
				if (CurrentAngle < BestAngle)
				{
					BestAngle = CurrentAngle;
					BestActor = CurrentActor;
				}
			}
		}
	}

	if (BestActor)
	{
		if (GrappleHookTarget)
		{
			GrappleHookTarget->ItemEffect->Deactivate();
		}
		GrappleHookTarget = Cast<AGrappleHookTarget>(BestActor);
		GrappleHookTarget->ItemEffect->Activate();
	}
	else
	{
		if (GrappleHookTarget)
			GrappleHookTarget->ItemEffect->Deactivate();
		GrappleHookTarget = nullptr;
	}
}



void UGrappleComponent::EquipHook()
{
	if(GrappleType != EGrappleType::EGT_Inactive) return;
	if (GrappleHookClass && OwnerCharacter && GrappleHookTarget)
	{
		USkeletalMeshComponent* SkeletalMesh = OwnerCharacter->GetMesh();
		SpawnLocation = SkeletalMesh->GetBoneLocation(TEXT("hand_r"));
		TargetLocation = GrappleHookTarget->GetActorLocation();
		FVector Direction = (TargetLocation - SpawnLocation).GetSafeNormal();

		FRotator SpawnRotation = Direction.Rotation();
		OwnerCharacter->SetActorRotation(SpawnRotation);

		
		if(OwnerCharacter->GetMovementComponent()->IsFalling())
		{
			UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
			if (AnimInstance && GrappleMontage)
			{
				AnimInstance->Montage_Play(GrappleMontage, 2.0f);
				AnimInstance->Montage_JumpToSection(FName("Fire_Air"), GrappleMontage);
				AnimInstance->OnMontageEnded.AddDynamic(this, &UGrappleComponent::OnMontageEnded);
			}
		}else
		{
			UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
			if (AnimInstance && GrappleMontage)
			{
				AnimInstance->Montage_Play(GrappleMontage, 2.0f);
				AnimInstance->Montage_JumpToSection(FName("Fire_Ground"), GrappleMontage);
				AnimInstance->OnMontageEnded.AddDynamic(this, &UGrappleComponent::OnMontageEnded);
			}
		}
		GrappleType = EGrappleType::EGT_Waiting;
	
	}
}

void UGrappleComponent::TimelineCallback(float Value)
{
	FVector StartLocation = OwnerCharacter->GetActorLocation();
	FVector T = GrappleHookTarget->GetActorLocation();
	FVector TargetLocation2 = FVector(T.X, T.Y, T.Z + 100.0f);
	FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation2, Value);
	UE_LOG(LogTemp, Warning, TEXT("Character Location: %s"), *NewLocation.ToString())
	OwnerCharacter->SetActorLocation(NewLocation);
}

void UGrappleComponent::TimelineFinishedCallback()
{
	CableComponent->SetVisibility(false);
	GrappleHook->AutoDestroy();
	Timeline->Deactivate();
	GrappleType = EGrappleType::EGT_Inactive;
}

void UGrappleComponent::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance && GrappleMontage)
	{
		AnimInstance->Montage_Play(GrappleMontage, 2.0f);
		AnimInstance->Montage_JumpToSection(SectionName, GrappleMontage);
	}
}

void UGrappleComponent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	USkeletalMeshComponent* SkeletalMesh = OwnerCharacter->GetMesh();
	SpawnLocation = SkeletalMesh->GetBoneLocation(TEXT("hand_r"));
	FVector Direction = (TargetLocation - SpawnLocation).GetSafeNormal();

	FRotator SpawnRotation = Direction.Rotation();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerCharacter;
	SpawnParams.Instigator = OwnerCharacter->GetInstigator();

	GrappleHook = GetWorld()->SpawnActor<AGrappleHook>(
	GrappleHookClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (GrappleHook)
	{
		if (CableComponent)
		{
			CableComponent->SetWorldLocation(SpawnLocation);
			CableComponent->SetVisibility(true);
			CableComponent->SetAttachEndTo(GrappleHook, NAME_None);
		}

		GrappleHook->GetItemMesh()->SetWorldRotation(SpawnRotation);
		GrappleHook->SetOwnerCharacter(OwnerCharacter);
		GrappleHook->SetCableComponent(CableComponent);
			
	}
		
	
	GrappleHook->SetTarget(GrappleHookTarget);
	GrappleType = EGrappleType::EGT_Firing;
}
