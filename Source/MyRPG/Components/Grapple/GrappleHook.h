// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "GrappleHook.generated.h"

class UTimelineComponent;
class UCableComponent;
class AMyRPGCharacter;
class AGrappleHookTarget;
class UProjectileMovementComponent;
/**
 * 
 */
UCLASS()
class MYRPG_API AGrappleHook : public AItem
{
private:
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AGrappleHookTarget* GrappleHookTarget = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AMyRPGCharacter* OwnerCharacter = nullptr;

public:
	UCableComponent* CableComponent;
	AGrappleHook();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* LerpCurve;
	UTimelineComponent* Timeline;

	void SetTarget(AGrappleHookTarget* Target);
	FORCEINLINE void SetOwnerCharacter(AMyRPGCharacter* OwnerRPGCharacter)
	{
		OwnerCharacter = OwnerRPGCharacter;
	};
	FORCEINLINE void SetCableComponent(UCableComponent* Cable)
	{
		CableComponent = Cable;
	};

	UFUNCTION()
	void AutoDestroy();

	UFUNCTION()
	void TimelineCallback(float Value);

	UFUNCTION()
	void TimelineFinishedCallback();
};
