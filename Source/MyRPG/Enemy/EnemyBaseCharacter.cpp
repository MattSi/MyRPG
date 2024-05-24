// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyBaseCharacter.h"

// Sets default values
AEnemyBaseCharacter::AEnemyBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add(TEXT("Enemy"));
}

// Called when the game starts or when spawned
void AEnemyBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
