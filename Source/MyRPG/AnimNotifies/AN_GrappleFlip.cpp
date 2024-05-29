// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_GrappleFlip.h"

#include "MyRPGCharacter.h"
#include "Components/Grapple/GrappleComponent.h"

void UAN_GrappleFlip::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AMyRPGCharacter* Player = Cast<AMyRPGCharacter>(MeshComp->GetOwner());
	if(!Player) return;
	if(!Player->GrappleComponent) return;
	Player->GrappleComponent->PlayFlipTimeline();
	
}
