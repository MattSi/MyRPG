// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_EffectID.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API UANS_EffectID : public UAnimNotifyState
{
private:
	GENERATED_BODY()

public:
	UANS_EffectID();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int EffectID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BlackboardKeyName;
	
	// virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	// virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
		const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;
};
