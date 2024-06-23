// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTT_CastSkill.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UBTT_CastSkill::UBTT_CastSkill()
{
	NodeName = "Cast Skill";
	PreferRootMotionScale = 1.0f;
	bNotifyTick = true;
}

void UBTT_CastSkill::ComputeRootMotionScale(ACharacter* AICharacter, float RootMotionTranslateSize)
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PlayerCharacter == nullptr) return;

	float DistanceToPlayer = FVector::Dist(AICharacter->GetActorLocation(), PlayerCharacter->GetActorLocation());
	
	UE_LOG(LogTemp, Warning, TEXT("Distance to Player: %f, Root Motion Translation Size : %f, RootMotionScale: %f"),
	       DistanceToPlayer, RootMotionTranslateSize, PreferRootMotionScale);
}

EBTNodeResult::Type UBTT_CastSkill::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto* const Controller = Cast<AAIController>(OwnerComp.GetAIOwner()))
	{
		if (auto* const Character = Cast<ACharacter>(Controller->GetPawn()))
		{
			if (auto* AnimInstance = Character->GetMesh()->GetAnimInstance())
			{
				if(MontageToPlay != nullptr)
				{
					FOnMontageEnded MontageEndedDelegate;
					MontageEndedDelegate.BindLambda([this, &OwnerComp](UAnimMontage* Montage, bool bInterrupted)
					{
						if (Montage == MontageToPlay)
						{
							// Assuming OwnerComp is a class member, which should be set in ExecuteTask
							FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
						}
					});
					
					// Calculate the root motion translation size of the montage
					if(MontageToPlay->HasRootMotion())
					{
						const float MontageLength = MontageToPlay->GetPlayLength();
						const float RootMotionTranslationSize = CalculateRootMotionDistance(MontageToPlay, 0, MontageLength);
						
						ComputeRootMotionScale(Character, RootMotionTranslationSize);
						Character->SetAnimRootMotionTranslationScale(PreferRootMotionScale);
					}
					
					AnimInstance->Montage_Play(MontageToPlay);
					AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);
					return EBTNodeResult::InProgress;
				}
				
			}
		}
	}
	return EBTNodeResult::Failed;
}


void UBTT_CastSkill::CalculateMontageDistances()
{
}

float UBTT_CastSkill::CalculateRootMotionDistance(UAnimMontage* Montage, float StartTime, float EndTime)
{
	if(!Montage)
	{
		return 0.0f;
	}

	FTransform RootTransformStart = Montage->ExtractRootMotionFromTrackRange(0.0f, 0.0f);
	FTransform RootTransformEnd = Montage->ExtractRootMotionFromTrackRange(0.0f, Montage->GetPlayLength());

	const FVector StartLocation = RootTransformStart.GetLocation();
	const FVector EndLocation = RootTransformEnd.GetLocation();
	
	return FVector::Dist(StartLocation, EndLocation);
}
