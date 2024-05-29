// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Grapple/GrappleHookTarget.h"

#include "NiagaraComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
AGrappleHookTarget::AGrappleHookTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void AGrappleHookTarget::BeginPlay()
{
	Super::BeginPlay();
	ItemState = EItemState::EIS_Ground;
	ItemEffect->Deactivate();
	WidgetComponent->SetVisibility(false);
}

