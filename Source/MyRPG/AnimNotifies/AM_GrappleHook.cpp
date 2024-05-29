// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AM_GrappleHook.h"

#include "MyRPGCharacter.h"
#include "Components/Grapple/GrappleComponent.h"

void UAM_GrappleHook::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AMyRPGCharacter* Player = Cast<AMyRPGCharacter>(MeshComp->GetOwner());
	if(!Player) return;
	if(!Player->GrappleComponent) return;
	Player->GrappleComponent->OnGrappleHookNotified(MeshComp, Animation);
	
}
