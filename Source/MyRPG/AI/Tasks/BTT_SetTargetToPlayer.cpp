// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTT_SetTargetToPlayer.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UBTT_SetTargetToPlayer::UBTT_SetTargetToPlayer()
{
	NodeName = "Set Target To Player";
}

EBTNodeResult::Type UBTT_SetTargetToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PlayerCharacter == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	BlackboardComp->SetValueAsObject(TargetKey.SelectedKeyName, PlayerCharacter);

	return EBTNodeResult::Succeeded;
}
