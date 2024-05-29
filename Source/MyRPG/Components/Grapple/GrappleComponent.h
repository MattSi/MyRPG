// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrappleType.h"
#include "Components/ActorComponent.h"
#include "GrappleComponent.generated.h"


class UTimelineComponent;
class AGrappleHook;
class UCableComponent;
class AGrappleHookTarget;
class AMyRPGCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYRPG_API UGrappleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrappleComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	
	AMyRPGCharacter* OwnerCharacter = nullptr;
	AGrappleHookTarget *GrappleHookTarget = nullptr;
	EGrappleType GrappleType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hook")
	UCurveFloat* LerpCurve;

	UTimelineComponent* Timeline;
	
	void Inactive();
	void Firing();
	void NearTarget();
	void FindBestTarget(TArray<FOverlapResult> &OverlapResults);
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxGrappleDistance = 1000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCableComponent* CableComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AGrappleHook> GrappleHookClass;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* GrappleMontage;
	
	void EquipHook();
	UFUNCTION()
	void TimelineCallback(float Value);
	UFUNCTION()
	void TimelineFinishedCallback();
	
	UFUNCTION()
	void OnGrappleHookNotified(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);
	FVector SpawnLocation;
	FVector TargetLocation;
	AGrappleHook* GrappleHook;
	UFUNCTION()
	void PlayFlipTimeline();
};
