// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SetTargetToPlayer.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class MYRPG_API UBTT_SetTargetToPlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_SetTargetToPlayer();
protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector TargetKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
