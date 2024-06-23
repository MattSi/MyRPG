// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NPCAIController.generated.h"

class UBlackboardComponent;
/**
 * 
 */
UCLASS()
class MYRPG_API ANPCAIController : public AAIController
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FName TargetActorKey;
	
	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetPlayerInBlackboard();

	ANPCAIController();
protected:
	virtual void BeginPlay() override;
};
