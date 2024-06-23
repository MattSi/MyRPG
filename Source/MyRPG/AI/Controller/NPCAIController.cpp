// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Controller/NPCAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void ANPCAIController::BeginPlay()
{
	Super::BeginPlay();

	SetPlayerInBlackboard();
}


void ANPCAIController::SetPlayerInBlackboard()
{
	

	// Get the player character
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	UBlackboardComponent* BlackBoardComp = GetBlackboardComponent();
	if (PlayerCharacter && BlackBoardComp)
	{
		// Set the target actor in the blackboard
		BlackBoardComp->SetValueAsObject(TargetActorKey, PlayerCharacter);
	}
}

ANPCAIController::ANPCAIController()
{
}
