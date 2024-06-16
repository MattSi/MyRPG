// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_CastSkill.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class MYRPG_API UBTT_CastSkill : public UBTTask_BlackboardBase
{

	GENERATED_BODY()

public:
	UBTT_CastSkill();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	UAnimMontage* MontageToPlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	float RootMotionScale;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	float MinScale;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	float MaxScale;

	UFUNCTION(BlueprintCallable, Category = "Montage")
	void ComputeRootMotionScale(ACharacter* AICharacter, float RootMotionTranslateSize);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	 float CalculateRootMotionDistance(UAnimMontage* Montage, float StartTime, float EndTime);
};
