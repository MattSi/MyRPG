// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Grapple/GrappleHook.h"

#include "CableComponent.h"
#include "GrappleHookTarget.h"
#include "MyRPGCharacter.h"
#include "NiagaraComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/GameplayStatics.h"

AGrappleHook::AGrappleHook()
{
	PrimaryActorTick.bCanEverTick = false;
	Timeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Timeline"));
}

void AGrappleHook::BeginPlay()
{
	Super::BeginPlay();
	ItemState = EItemState::EIS_Ground;
	ItemEffect->Deactivate();

	if (LerpCurve)
	{
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this, FName("TimelineCallback"));
		Timeline->AddInterpFloat(LerpCurve, TimelineCallback);

		FOnTimelineEventStatic TimelineFinishedCallback;
		TimelineFinishedCallback.BindUFunction(this, FName("TimelineFinishedCallback"));
		Timeline->SetTimelineFinishedFunc(TimelineFinishedCallback);
	}
	
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGrappleHook::AutoDestroy, 3.0f, false);
	});
	
	//UGameplayStatics::ApplyDamage()
}

void AGrappleHook::SetTarget(AGrappleHookTarget* Target)
{
	GrappleHookTarget = Target;
	if (Timeline && !Timeline->IsPlaying())
	{
		Timeline->PlayFromStart();
	}
}


void AGrappleHook::AutoDestroy()
{
	if(CableComponent)
	{
		CableComponent->SetVisibility(false);
	}
	Destroy();
}

void AGrappleHook::TimelineCallback(float Value)
{
	FVector StartLocation = OwnerCharacter->GetActorLocation();
	FVector TargetLocation = GrappleHookTarget->GetActorLocation();
	FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, Value);
	SetActorLocation(NewLocation);
}

void AGrappleHook::TimelineFinishedCallback()
{
}

