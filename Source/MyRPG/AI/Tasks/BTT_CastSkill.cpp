// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTT_CastSkill.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UBTT_CastSkill::UBTT_CastSkill()
{
	NodeName = "Cast Skill";
	RootMotionScale = 1.0f;
	MinScale = 0.3f;
	MaxScale = 1.6f;
	bNotifyTick = true;
}

void UBTT_CastSkill::ComputeRootMotionScale(ACharacter* AICharacter, float RootMotionTranslateSize)
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PlayerCharacter == nullptr) return;

	float DistanceToPlayer = FVector::Dist(AICharacter->GetActorLocation(), PlayerCharacter->GetActorLocation());
	if (RootMotionTranslateSize > 0)
	{
		RootMotionScale = DistanceToPlayer / RootMotionTranslateSize;
		RootMotionScale = FMath::Clamp(RootMotionScale, MinScale, MaxScale);
	}
	else
	{
		RootMotionScale = 1.0f;
	}
	UE_LOG(LogTemp, Log, TEXT("Distance to Player: %f, Root Motion Translation Size : %f, RootMotionScale: %f"),
	       DistanceToPlayer, RootMotionTranslateSize, RootMotionScale);
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
						float MontageLength = MontageToPlay->GetPlayLength();
						float RootMotionTranslationSize = CalculateRootMotionDistance(MontageToPlay, 0, MontageLength);

						ComputeRootMotionScale(Character, RootMotionTranslationSize);
						Character->SetAnimRootMotionTranslationScale(RootMotionScale);
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

float UBTT_CastSkill::CalculateRootMotionDistance(UAnimMontage* Montage, float StartTime, float EndTime)
{
	if(!Montage) return 0.0f;

	FTransform RootMotionTransform = Montage->ExtractRootMotion(StartTime, EndTime, false);
	return RootMotionTransform.GetTranslation().Size();
}

void UBTT_CastSkill::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp)
	{
		int32 BlackboardValue = BlackboardComp->GetValueAsInt(BlackboardKey.SelectedKeyName);
		if (AbortValues.Contains(BlackboardValue))
		{
			if (auto* const Controller = Cast<AAIController>(OwnerComp.GetAIOwner()))
			{
				if (auto* const Character = Cast<ACharacter>(Controller->GetPawn()))
				{
					if (auto* AnimInstance = Character->GetMesh()->GetAnimInstance())
					{
						if(MontageToPlay )
						{
							AnimInstance->Montage_Stop(0, MontageToPlay);
						}
					}
				}
			}
			FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
		}
	}
}
