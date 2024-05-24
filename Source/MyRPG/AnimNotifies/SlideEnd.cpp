// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/SlideEnd.h"

#include "MyRPGCharacter.h"

void USlideEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AMyRPGCharacter* Player = Cast<AMyRPGCharacter>(MeshComp->GetOwner());
	if(!Player) return;

	Player->DodgeMontageEnded();
}
