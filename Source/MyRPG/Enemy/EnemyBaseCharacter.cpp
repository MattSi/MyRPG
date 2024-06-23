// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyBaseCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

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

	CalculateMontageDistances();
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

void AEnemyBaseCharacter::CalculateMontageDistances()
{
	for (UAnimMontage* Montage : Montages)
	{
		if (Montage)
		{
			FTransform RootTransformStart = Montage->ExtractRootMotionFromTrackRange(0.0f, 0.0f);
			FTransform RootTransformEnd = Montage->ExtractRootMotionFromTrackRange(0.0f, Montage->GetPlayLength());

			const FVector StartLocation = RootTransformStart.GetLocation();
			const FVector EndLocation = RootTransformEnd.GetLocation();
			float Distance = FVector::Dist(StartLocation, EndLocation);

			UE_LOG(LogTemp, Log, TEXT("Distance for Montage %s: %f"), *Montage->GetName(), Distance);
		}
	}
}



