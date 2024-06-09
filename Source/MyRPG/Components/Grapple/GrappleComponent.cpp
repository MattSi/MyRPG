// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Grapple/GrappleComponent.h"

#include "CableComponent.h"
#include "GrappleHook.h"
#include "GrappleHookTarget.h"
#include "MyRPGCharacter.h"
#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/WidgetComponent.h"
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
		CableComponent->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::KeepRelativeTransform,
		                                  FName("WeaponRightHand"));
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
	FName Section = (rand() % 2) ? TEXT("Flip1") : TEXT("Flip2");
	OwnerCharacter->PlayAnimMontage(GrappleMontage, 1.8f, Section);
	GrappleType = EGrappleType::EGT_Waiting;
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
			GrappleHookTarget->WidgetComponent->SetVisibility(false);

		GrappleHookTarget = Cast<AGrappleHookTarget>(BestActor);
		GrappleHookTarget->WidgetComponent->SetVisibility(true);
	}
	else
	{
		if (GrappleHookTarget)
			GrappleHookTarget->WidgetComponent->SetVisibility(false);
		GrappleHookTarget = nullptr;
	}
}


void UGrappleComponent::EquipHook()
{
	if (GrappleType != EGrappleType::EGT_Inactive) return;
	if (GrappleHookClass && OwnerCharacter && GrappleHookTarget)
	{
		USkeletalMeshComponent* SkeletalMesh = OwnerCharacter->GetMesh();
		SpawnLocation = SkeletalMesh->GetBoneLocation(TEXT("hand_r"));
		TargetLocation = GrappleHookTarget->GetActorLocation();
		FVector Direction = (TargetLocation - SpawnLocation).GetSafeNormal();

		FRotator SpawnRotation = Direction.Rotation();
		OwnerCharacter->SetActorRotation(SpawnRotation);

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
				CableComponent->SetAttachEndTo(GrappleHook, NAME_None);
			}

			GrappleHook->GetItemMesh()->SetWorldRotation(SpawnRotation);
			GrappleHook->SetOwnerCharacter(OwnerCharacter);
			GrappleHook->SetCableComponent(CableComponent);
			GrappleHook->GetItemMesh()->SetVisibility(false);

			if (OwnerCharacter->GetMovementComponent()->IsFalling())
			{
				OwnerCharacter->PlayAnimMontage(GrappleMontage, 1.5f, FName("Fire_Air"));
			}
			else
			{
				OwnerCharacter->PlayAnimMontage(GrappleMontage, 1.5f, FName("Fire_Ground"));
			}

			GrappleType = EGrappleType::EGT_Waiting;
		}
		else
		{
			OwnerCharacter->ChangeActionState(EActionState::EAS_Idle);
		}
	}
	else
	{
		OwnerCharacter->ChangeActionState(EActionState::EAS_Idle);
	}
}

void UGrappleComponent::TimelineCallback(float Value)
{
	FVector StartLocation = OwnerCharacter->GetActorLocation();
	FVector T = GrappleHookTarget->GetActorLocation();
	FVector TargetLocation2 = FVector(T.X, T.Y, T.Z + 100.0f);
	FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation2, Value);
	OwnerCharacter->SetActorLocation(NewLocation);
}

void UGrappleComponent::TimelineFinishedCallback()
{
	CableComponent->SetVisibility(false);
	GrappleHook->AutoDestroy();
	Timeline->Deactivate();
	GrappleType = EGrappleType::EGT_Inactive;
	OwnerCharacter->ChangeActionState(EActionState::EAS_Idle);
}

void UGrappleComponent::OnGrappleHookNotified(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if(CableComponent)
		CableComponent->SetVisibility(true);
	GrappleHook->GetItemMesh()->SetVisibility(true);
	GrappleHook->SetTarget(GrappleHookTarget);
	GrappleType = EGrappleType::EGT_Firing;
}

void UGrappleComponent::PlayFlipTimeline()
{
	if (!Timeline->IsPlaying())
	{
		Timeline->PlayFromStart();
	}
}
