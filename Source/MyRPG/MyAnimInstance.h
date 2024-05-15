// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LocomotionTypes.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "MyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	protected:
	UFUNCTION(BlueprintPure, Category = "Distance Matching", meta = (BlueprintThreadSafe))
	bool ShouldDistanceMatchStop() const;

	UFUNCTION(BlueprintPure, Category = "Distance Matching", meta = (BlueprintThreadSafe))
	double GetPredictedStopDistance() const;

	UFUNCTION(BlueprintPure, Category = "Idle Breaks", meta = (BlueprintThreadSafe))
	bool CanPlayIdleBreak() const;

	UFUNCTION(BlueprintCallable, Category = "State Node Function", meta = (BlueprintThreadSafe))
	void SetupIdleState(const struct FAnimUpdateContext& Context, const struct FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "State Node Function", meta = (BlueprintThreadSafe))
	void UpdateIdleState(const struct FAnimUpdateContext& Context, const struct FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "State Node Function", meta = (BlueprintThreadSafe))
	void SetupStartState(const struct FAnimUpdateContext& Context, const struct FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "State Node Function", meta = (BlueprintThreadSafe))
	void UpdateStartState(const struct FAnimUpdateContext& Context, const struct FAnimNodeReference& Node);
	
	UFUNCTION(BlueprintCallable, Category = "State Node Functions", meta = (BlueprintThreadSafe))
	void UpdateStopState(const struct FAnimUpdateContext& Context, const struct FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "State Node Functions", meta = (BlueprintThreadSafe))
	void SetupPivotState(const struct FAnimUpdateContext& Context, const struct FAnimNodeReference& Node);
	
	UFUNCTION(BlueprintCallable, Category = "State Node Functions", meta = (BlueprintThreadSafe))
	void UpdatePivotState(const struct FAnimUpdateContext& Context, const struct FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "Anim Node Functions", meta = (BlueprintThreadSafe))
	void UpdateIdleAnim(const struct FAnimUpdateContext& Context, const struct FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "Anim Node Functions", meta = (BlueprintThreadSafe))
	void SetupIdleBreakAnim(const struct FAnimUpdateContext& Context, const struct FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "Anim Node Functions", meta = (BlueprintThreadSafe))
	void SetupStartAnim(const struct FAnimUpdateContext& Context, const struct FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "Anim Node Functions", meta = (BlueprintThreadSafe))
	void UpdateStartAnim(const struct FAnimUpdateContext& Context, const struct FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "Anim Node Functions", meta = (BlueprintThreadSafe))
	void UpdateCycleAnim(const struct FAnimUpdateContext& Context, const struct FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "Anim Node Functions", meta = (BlueprintThreadSafe))
	void SetupStopAnim(const struct FAnimUpdateContext& Context, const struct FAnimNodeReference& Node);
	
	UFUNCTION(BlueprintCallable, Category = "Anim Node Functions", meta = (BlueprintThreadSafe))
	void UpdateStopAnim(const struct FAnimUpdateContext& Context, const struct FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "Anim Node Functions", meta = (BlueprintThreadSafe))
	void SetupFallLandAnim(const struct FAnimUpdateContext& Context, const struct FAnimNodeReference& Node);
	
	UFUNCTION(BlueprintCallable, Category = "Anim Node Functions", meta = (BlueprintThreadSafe))
	void UpdateFallLandAnim(const struct FAnimUpdateContext& Context, const struct FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "Anim Node Functions", meta = (BlueprintThreadSafe))
	void SetupPivotAnim(const struct FAnimUpdateContext& Context, const struct FAnimNodeReference& Node);
	
	UFUNCTION(BlueprintCallable, Category = "Anim Node Functions", meta = (BlueprintThreadSafe))
	void UpdatePivotAnim(const struct FAnimUpdateContext& Context, const struct FAnimNodeReference& Node);
	
	float GetGroundDistance(TObjectPtr<ACharacter> Owner);
	void UpdateLocationData(float DeltaTime);
	void UpdateRotationData(float DeltaTime);
	void UpdateVelocityData();
	void UpdateAccelerationData();
	void UpdateRootYawOffset(float InDeltaTime);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Settings")
	FName LocomotionDistanceCurveName = TEXT("Distance");

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Settings")
	FName JumpDistanceCurveName = TEXT("GroundDistance");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Location Data")
	FVector WorldLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Location Data")
	float DisplacementSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rotation Data")
	FRotator WorldRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rotation Data")
	float AdditiveLeanAngle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Velocity Data")
	FVector WorldVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Velocity Data")
	FVector LocalVelocity2D;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Velocity Data")
	float LocalVelocityDirectionAngle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Velocity Data")
	float LocalVelocityDirectionAngleWithOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Velocity Data")
	ECardinalDirection LocalVelocityDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Velocity Data")
	uint8 bHasVelocity : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Acceleration Data")
	FVector LocalAcceleration2D;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Acceleration Data")
	uint8 bHasAcceleration : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Idle Breaks")
	float TimeUntilNextIdleBreak;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place")
	float RootYawOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stride Warping")
	float StrideWarpingStartAlpha;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stride Warping")
	float StrideWarpingCycleAlpha;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stride Warping")
	float StrideWarpingPivotAlpha;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character State Data")
	float GroundDistance = -1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character State Data")
	float TimeToJumpApex = -1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character State Data")
	uint8 bIsRunningIntoWall : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character State Data")
	uint8 bIsOnGround : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character State Data")
	uint8 bIsJumping : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character State Data")
	uint8 bIsFalling : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Anim Set - Idle")
	TObjectPtr<UAnimSequence> IdleAnimSequence;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Anim Set - Idle")
	TArray<TObjectPtr<UAnimSequence>> IdleBreakAnimSequences;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Anim Set - Start")
	FCardinalDirections JogStartCardinals;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Anim Set - Jog")
	FCardinalDirections JogCardinals;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Anim Set - Stops")
	FCardinalDirections JogStopCardinals;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Anim Set - Pivots")
	FCardinalDirections JogPivotCardinals;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Anim Set - Jump")
	TObjectPtr<UAnimSequence> JumpStart;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Anim Set - Jump")
	TObjectPtr<UAnimSequence> JumpStartLoop;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Anim Set - Jump")
	TObjectPtr<UAnimSequence> JumpApex;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Anim Set - Jump")
	TObjectPtr<UAnimSequence> JumpFallLand;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Anim Set - Jump")
	TObjectPtr<UAnimSequence> JumpFallLoop;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Anim Set - Jump")
	TObjectPtr<UAnimSequence> JumpRecoveryAdditive;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LocomotionSM Data")
	ECardinalDirection StartDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LocomotionSM Data")
	float LastPivotTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pivots")
	FVector PivotStartingAcceleration;
	
	void SetRootYawOffset(float InRootYawOffset);
private:
	static TObjectPtr<UAnimSequence> SelectDirectionalAnimation(const FCardinalDirections& Cardinals,
																ECardinalDirection Direction);
	static ECardinalDirection SelectCardinalDirectionFromAngle(float Angle, float DeadZone,
															   ECardinalDirection CurrentDirection,
															   bool bUseCurrentDirection);
	static ECardinalDirection GetOppositeCardinalDirection(ECardinalDirection CurrentDirection);

	bool bIsFirstUpdate = false;
	bool bWasMovingLastUpdate = false;
	bool bIsAnyMontagePlaying = false;
	bool bUseSeparateBrakingFriction = false;
	float GroundFriction = 0;
	float BrakingFriction = 0;
	float BrakingFrictionFactor = 0;
	float BrakingDecelerationWalking = 0;
	FRotator PreWorldRotation;
	FVector CurrentAcceleration{0};
	FVector LastUpdatedVelocity{0};

	// Location Data
	float DisplacementSinceLastUpdate = 0;
	FVector PreWorldLocation{0};

	// Rotation Data
	float YawDeltaSinceLastUpdate = 0;

	// Velocity Data
	ECardinalDirection LocalVelocityDirectionNoOffset;

	// Acceleration Data
	FVector PivotDirection2D{0};

	// Locomotion SM Data
	ECardinalDirection PivotInitialDirection;
	ECardinalDirection CardinalDirectionFromAcceleration;

	// Turn In Place
	ERootYawOffsetMode RootYawOffsetMode;
	FVector2D RootYawOffsetAngleClamp{-120, 100};
	FFloatSpringState RootYawOffsetSpringState;

	// Idle Breaks
	float IdleBreakDelayTime = 0;
	uint8 CurrentIdleBreakIndex = 0;

	// Setting
	float CardinalDirectionDeadZone = 10.f;
	float StrideWarpingBlendInStartOffset = 0.15f;
	float StrideWarpingBlendInDurationScaled = 0.2f;
	FVector2D PlayRateClampCycle{0.8f, 1.2f};
	FVector2D PlayRateClampStartsPivots{0.6f, 5.0f};


	// Pivots
	float TimeAtPivotStop = 0;

	// Ground Distance
	uint64 LastUpdateFrame = 0;
	float LastGroundDistance = 0;

	// Jump
	float TimeFalling = 0;
};
