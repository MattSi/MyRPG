// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_EffectID.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UANS_EffectID::UANS_EffectID()
{
	
}

void UANS_EffectID::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if(MeshComp && MeshComp->GetOwner())
	{
		AAIController* AIController = Cast<AAIController>(MeshComp->GetOwner()->GetInstigatorController());

		if(AIController && AIController->GetBlackboardComponent())
		{
			AIController->GetBlackboardComponent()->SetValueAsInt(BlackboardKeyName, EffectID);
			UE_LOG(LogTemp, Log, TEXT("Set Blackboard key %s to %d"), *BlackboardKeyName.ToString(), EffectID);
		}
	}
}

void UANS_EffectID::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if(MeshComp && MeshComp->GetOwner())
	{
		AAIController* AIController = Cast<AAIController>(MeshComp->GetOwner()->GetInstigatorController());

		if(AIController && AIController->GetBlackboardComponent())
		{
			AIController->GetBlackboardComponent()->SetValueAsInt(BlackboardKeyName, 0);
			UE_LOG(LogTemp, Log, TEXT("Set Blackboard key %s to 0"), *BlackboardKeyName.ToString());
		}
	}
}

FString UANS_EffectID::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("Effect ID: %d"), EffectID);
}
