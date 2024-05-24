// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AttackEnd.h"

#include "MyRPGCharacter.h"

void UAttackEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{

	AMyRPGCharacter* Player = Cast<AMyRPGCharacter>(MeshComp->GetOwner());
	if(!Player) return;

	Player->AttackEnd();
}
