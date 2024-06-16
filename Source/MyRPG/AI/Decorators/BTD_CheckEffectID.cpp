// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorators/BTD_CheckEffectID.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTD_CheckEffectID::UBTD_CheckEffectID()
{
	NodeName = "Check Effect ID";
}

bool UBTD_CheckEffectID::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if(const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent())
	{
		int32 EffectID = BlackboardComp->GetValueAsInt(EffectIDKey.SelectedKeyName);
		return EffectID == TargetEffectID;
	}

	return false;
}

void UBTD_CheckEffectID::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	if(CalculateRawConditionValue(OwnerComp, NodeMemory))
	{
		OwnerComp.RequestExecution(this);
	}
}

void UBTD_CheckEffectID::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	if(CalculateRawConditionValue(OwnerComp, NodeMemory))
	{
		OwnerComp.RequestExecution(this);
	}
}


FString UBTD_CheckEffectID::GetStaticDescription() const
{
	return FString::Printf(TEXT("Check if Effect ID == %d"), TargetEffectID);

}
