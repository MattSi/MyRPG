// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_CheckEffectID.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API UBTD_CheckEffectID : public UBTDecorator
{

	GENERATED_BODY()

public:
	UBTD_CheckEffectID();

	virtual FString GetStaticDescription() const override;
protected:
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector EffectIDKey;

	UPROPERTY(EditAnywhere, Category = "Condition")
	int TargetEffectID;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
