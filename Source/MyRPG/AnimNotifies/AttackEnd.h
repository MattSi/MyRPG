// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AttackEnd.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API UAttackEnd : public UAnimNotify
{
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

private:
	GENERATED_BODY()
};
