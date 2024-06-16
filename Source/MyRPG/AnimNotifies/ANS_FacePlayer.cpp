// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_FacePlayer.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void UANS_FacePlayer::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		// Get the player character
		ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(Character->GetWorld(), 0);
		if (PlayerCharacter)
		{
			// Calculate direction to player
			FVector DirectionToPlayer = (PlayerCharacter->GetActorLocation() - Character->GetActorLocation()).GetSafeNormal();

			// Calculate the rotation needed to face the player
			FRotator CurrentRotation = Character->GetActorRotation();
			FRotator TargetRotation = DirectionToPlayer.Rotation();
			TargetRotation.Pitch = CurrentRotation.Pitch;
			TargetRotation.Roll = CurrentRotation.Roll;

			// Set the character's rotation to face the player
			Character->SetActorRotation(TargetRotation);
		}
	}
}

void UANS_FacePlayer::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
	if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		// Get the player character
		ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(Character->GetWorld(), 0);
		if (PlayerCharacter)
		{
			// Calculate direction to player
			FVector DirectionToPlayer = (PlayerCharacter->GetActorLocation() - Character->GetActorLocation()).GetSafeNormal();

			// Calculate the rotation needed to face the player
			FRotator CurrentRotation = Character->GetActorRotation();
			FRotator TargetRotation = DirectionToPlayer.Rotation();
			TargetRotation.Pitch = CurrentRotation.Pitch;
			TargetRotation.Roll = CurrentRotation.Roll;

			// Set the character's rotation to face the player
			Character->SetActorRotation(TargetRotation);
		}
	}
}
