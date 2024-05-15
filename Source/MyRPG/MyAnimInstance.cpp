// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"

#include "AnimationStateMachineLibrary.h"
#include "AnimCharacterMovementLibrary.h"
#include "AnimDistanceMatchingLibrary.h"
#include "AnimExecutionContextLibrary.h"
#include "KismetAnimationLibrary.h"
#include "SequenceEvaluatorLibrary.h"
#include "SequencePlayerLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (TObjectPtr<ACharacter> Owner = Cast<ACharacter>(GetOwningActor()))
	{
		CurrentAcceleration = Owner->GetCharacterMovement()->GetCurrentAcceleration();
		PreWorldLocation = WorldLocation;
		WorldLocation = Owner->GetActorLocation();
		PreWorldRotation = WorldRotation;
		WorldRotation = Owner->GetActorRotation();
		WorldVelocity = Owner->GetVelocity();
		LastUpdatedVelocity = Owner->GetCharacterMovement()->GetLastUpdateVelocity();

		bIsAnyMontagePlaying = IsAnyMontagePlaying();
		bUseSeparateBrakingFriction = Owner->GetCharacterMovement()->bUseSeparateBrakingFriction;
		bIsOnGround = Owner->GetCharacterMovement()->IsMovingOnGround();
		bIsJumping = Owner->GetCharacterMovement()->MovementMode == MOVE_Falling && WorldVelocity.Z > 0;
		bIsFalling = Owner->GetCharacterMovement()->MovementMode == MOVE_Falling && WorldVelocity.Z <= 0;

		BrakingDecelerationWalking = Owner->GetCharacterMovement()->BrakingDecelerationWalking;
		BrakingFrictionFactor = Owner->GetCharacterMovement()->BrakingFrictionFactor;
		BrakingFriction = Owner->GetCharacterMovement()->BrakingFriction;
		GroundFriction = Owner->GetCharacterMovement()->GroundFriction;

		TimeToJumpApex = bIsJumping ? -WorldVelocity.Z / Owner->GetCharacterMovement()->GetGravityZ() : 0;
		TimeFalling = bIsFalling ? TimeFalling + DeltaSeconds : bIsJumping ? 0 : TimeFalling;

		GroundDistance = GetGroundDistance(Owner);
	}
}

void UMyAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	UpdateLocationData(DeltaSeconds);
	UpdateRotationData(DeltaSeconds);
	UpdateVelocityData();
	UpdateAccelerationData();
	UpdateRootYawOffset(DeltaSeconds);

	bIsFirstUpdate = false;
}

bool UMyAnimInstance::ShouldDistanceMatchStop() const
{
	return bHasVelocity && !bHasAcceleration;
}

double UMyAnimInstance::GetPredictedStopDistance() const
{
	return UAnimCharacterMovementLibrary::PredictGroundMovementStopLocation(
		LastUpdatedVelocity,
		bUseSeparateBrakingFriction,
		BrakingFriction,
		GroundFriction,
		BrakingFrictionFactor,
		BrakingDecelerationWalking).Size2D();
}

bool UMyAnimInstance::CanPlayIdleBreak() const
{
	return !IdleBreakAnimSequences.IsEmpty() && !(bIsAnyMontagePlaying || bHasVelocity);
}

void UMyAnimInstance::SetupIdleState(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	IdleBreakDelayTime = FMath::TruncToInt(FMath::Abs(WorldLocation.X + WorldLocation.Y)) % 10 + 6;
	TimeUntilNextIdleBreak = IdleBreakDelayTime;
}

void UMyAnimInstance::UpdateIdleState(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FAnimationStateResultReference AnimationState;
	EAnimNodeReferenceConversionResult ConversionResult;
	UAnimationStateMachineLibrary::ConvertToAnimationStateResult(Node, AnimationState, ConversionResult);
	if (ConversionResult == EAnimNodeReferenceConversionResult::Succeeded)
	{
		if (!UAnimationStateMachineLibrary::IsStateBlendingOut(Context, AnimationState))
		{
			if (CanPlayIdleBreak())
			{
				TimeUntilNextIdleBreak -= UAnimExecutionContextLibrary::GetDeltaTime(Context);
			}
			else
			{
				TimeUntilNextIdleBreak -= IdleBreakDelayTime;
			}
		}
	}
}

void UMyAnimInstance::SetupStartState(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	StartDirection = LocalVelocityDirection;
}

void UMyAnimInstance::UpdateStartState(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FAnimationStateResultReference AnimationState;
	EAnimNodeReferenceConversionResult ConversionResult;
	UAnimationStateMachineLibrary::ConvertToAnimationStateResult(Node, AnimationState, ConversionResult);
	if (ConversionResult == EAnimNodeReferenceConversionResult::Succeeded)
	{
		if (!UAnimationStateMachineLibrary::IsStateBlendingOut(Context, AnimationState))
		{
			RootYawOffsetMode = ERootYawOffsetMode::ERYOM_Hold;
		}
	}
}

void UMyAnimInstance::UpdateStopState(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FAnimationStateResultReference AnimationState;
	EAnimNodeReferenceConversionResult ConversionResult;
	UAnimationStateMachineLibrary::ConvertToAnimationStateResult(Node, AnimationState, ConversionResult);
	if (ConversionResult == EAnimNodeReferenceConversionResult::Succeeded)
	{
		if (!UAnimationStateMachineLibrary::IsStateBlendingOut(Context, AnimationState))
		{
			RootYawOffsetMode = ERootYawOffsetMode::ERYOM_Accumulate;
		}
	}
}

void UMyAnimInstance::SetupPivotState(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	PivotInitialDirection = LocalVelocityDirection;
}

void UMyAnimInstance::UpdatePivotState(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if(LastPivotTime>0)
	{
		LastPivotTime -= UAnimExecutionContextLibrary::GetDeltaTime(Context);
	}
}

void UMyAnimInstance::UpdateIdleAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	EAnimNodeReferenceConversionResult ConversionResult;
	const FSequencePlayerReference SequencePlayer = USequencePlayerLibrary::ConvertToSequencePlayer(
		Node, ConversionResult);
	if (ConversionResult == EAnimNodeReferenceConversionResult::Succeeded)
	{
		USequencePlayerLibrary::SetSequenceWithInertialBlending(Context, SequencePlayer, IdleAnimSequence);
	}
}


void UMyAnimInstance::SetupIdleBreakAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	EAnimNodeReferenceConversionResult ConversionResult;
	const FSequencePlayerReference SequencePlayer = USequencePlayerLibrary::ConvertToSequencePlayer(
		Node, ConversionResult);
	if (ConversionResult == EAnimNodeReferenceConversionResult::Succeeded)
	{
		USequencePlayerLibrary::SetSequence(SequencePlayer, IdleBreakAnimSequences[CurrentIdleBreakIndex]);
		CurrentIdleBreakIndex = (CurrentIdleBreakIndex + 1) % IdleBreakAnimSequences.Num();
	}
}

void UMyAnimInstance::SetupStartAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	EAnimNodeReferenceConversionResult ConversionResult;
	const FSequenceEvaluatorReference SequenceEvaluator = USequenceEvaluatorLibrary::ConvertToSequenceEvaluator(
		Node, ConversionResult);
	if (ConversionResult == EAnimNodeReferenceConversionResult::Succeeded)
	{
		USequenceEvaluatorLibrary::SetSequence(
			SequenceEvaluator, SelectDirectionalAnimation(JogStartCardinals, LocalVelocityDirection));
		USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, 0);
		StrideWarpingStartAlpha = 0;
	}
}

void UMyAnimInstance::UpdateStartAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	EAnimNodeReferenceConversionResult ConversionResult;
	const FSequenceEvaluatorReference SequenceEvaluator = USequenceEvaluatorLibrary::ConvertToSequenceEvaluator(
		Node, ConversionResult);
	if (ConversionResult == EAnimNodeReferenceConversionResult::Succeeded)
	{
		const float ExplicitTime = USequenceEvaluatorLibrary::GetAccumulatedTime(SequenceEvaluator);
		StrideWarpingStartAlpha = FMath::GetMappedRangeValueClamped(FVector2D(0, StrideWarpingBlendInDurationScaled),
		                                                            FVector2D(0, 1),
		                                                            ExplicitTime - StrideWarpingBlendInStartOffset);

		const FVector2D PlayRateClamp(
			UKismetMathLibrary::Lerp(StrideWarpingBlendInDurationScaled, PlayRateClampStartsPivots.X,
			                         StrideWarpingStartAlpha), PlayRateClampStartsPivots.Y);
		UAnimDistanceMatchingLibrary::AdvanceTimeByDistanceMatching(Context, SequenceEvaluator,
		                                                            DisplacementSinceLastUpdate,
		                                                            LocomotionDistanceCurveName, PlayRateClamp);
	}
}

void UMyAnimInstance::UpdateCycleAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	EAnimNodeReferenceConversionResult ConversionResult;
	const FSequencePlayerReference SequencePlayer = USequencePlayerLibrary::ConvertToSequencePlayer(
		Node, ConversionResult);
	if (ConversionResult == EAnimNodeReferenceConversionResult::Succeeded)
	{
		USequencePlayerLibrary::SetSequenceWithInertialBlending(
			Context, SequencePlayer, SelectDirectionalAnimation(JogCardinals, LocalVelocityDirectionNoOffset));

		UAnimDistanceMatchingLibrary::SetPlayrateToMatchSpeed(SequencePlayer, DisplacementSpeed, PlayRateClampCycle);

		StrideWarpingCycleAlpha = FMath::FInterpTo(
			StrideWarpingCycleAlpha, bIsRunningIntoWall ? 0.5f : 1.0f,
			UAnimExecutionContextLibrary::GetDeltaTime(Context), 10);
	}
}

void UMyAnimInstance::SetupStopAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	EAnimNodeReferenceConversionResult ConversionResult;
	const FSequenceEvaluatorReference SequenceEvaluator = USequenceEvaluatorLibrary::ConvertToSequenceEvaluator(
		Node, ConversionResult);
	if (ConversionResult == EAnimNodeReferenceConversionResult::Succeeded)
	{
		USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator,
		                                       SelectDirectionalAnimation(JogStopCardinals, LocalVelocityDirection));
	}

	if (!ShouldDistanceMatchStop())
	{
		UAnimDistanceMatchingLibrary::DistanceMatchToTarget(SequenceEvaluator, 0, LocomotionDistanceCurveName);
	}
}

void UMyAnimInstance::UpdateStopAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (ShouldDistanceMatchStop())
	{
		const double DistanceToMatch = GetPredictedStopDistance();
		if (DistanceToMatch > 0)
		{
			EAnimNodeReferenceConversionResult ConversionResult;
			const FSequenceEvaluatorReference SequenceEvaluator = USequenceEvaluatorLibrary::ConvertToSequenceEvaluator(
				Node, ConversionResult);
			if (ConversionResult == EAnimNodeReferenceConversionResult::Succeeded)
			{
				UAnimDistanceMatchingLibrary::DistanceMatchToTarget(SequenceEvaluator, DistanceToMatch,
				                                                    LocomotionDistanceCurveName);
			}
			return;
		}
	}

	EAnimNodeReferenceConversionResult ConversionResult;
	const FSequenceEvaluatorReference SequenceEvaluator = USequenceEvaluatorLibrary::ConvertToSequenceEvaluator(
		Node, ConversionResult);
	if (ConversionResult == EAnimNodeReferenceConversionResult::Succeeded)
	{
		USequenceEvaluatorLibrary::AdvanceTime(Context, SequenceEvaluator);
	}
}

void UMyAnimInstance::SetupFallLandAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	EAnimNodeReferenceConversionResult ConversionResult;
	const FSequenceEvaluatorReference SequenceEvaluator = USequenceEvaluatorLibrary::ConvertToSequenceEvaluator(
		Node, ConversionResult);
	if (ConversionResult == EAnimNodeReferenceConversionResult::Succeeded)
	{
		USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, 0);
	}
}

void UMyAnimInstance::UpdateFallLandAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	EAnimNodeReferenceConversionResult ConversionResult;
	const FSequenceEvaluatorReference SequenceEvaluator = USequenceEvaluatorLibrary::ConvertToSequenceEvaluator(
		Node, ConversionResult);
	if (ConversionResult == EAnimNodeReferenceConversionResult::Succeeded)
	{
		UAnimDistanceMatchingLibrary::DistanceMatchToTarget(SequenceEvaluator, GroundDistance, JumpDistanceCurveName);
	}
}

void UMyAnimInstance::SetupPivotAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	PivotStartingAcceleration = LocalAcceleration2D;

	EAnimNodeReferenceConversionResult ConversionResult;
	const FSequenceEvaluatorReference SequenceEvaluator = USequenceEvaluatorLibrary::ConvertToSequenceEvaluator(Node, ConversionResult);
	if (ConversionResult == EAnimNodeReferenceConversionResult::Succeeded)
	{
		USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, SelectDirectionalAnimation(JogPivotCardinals, CardinalDirectionFromAcceleration));
		USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, 0);
		StrideWarpingPivotAlpha = 0;
		TimeAtPivotStop = 0;
		LastPivotTime = 0.2;
	}
}

void UMyAnimInstance::UpdatePivotAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
}

float UMyAnimInstance::GetGroundDistance(TObjectPtr<ACharacter> Owner)
{
	static constexpr float GroundTraceDistance = 100000.0f;
	const TObjectPtr<UCharacterMovementComponent> MoveComponent = Owner->GetCharacterMovement();

	if (!MoveComponent || (GFrameCounter == LastUpdateFrame))
	{
		return LastGroundDistance;
	}

	if (MoveComponent->MovementMode == MOVE_Walking)
	{
		LastGroundDistance = 0.0f;
	}
	else
	{
		UCapsuleComponent* CapsuleComp = Owner->GetCapsuleComponent();
		check(CapsuleComp);

		const float CapsuleHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();
		const ECollisionChannel CollisionChannel = (MoveComponent->UpdatedComponent
			                                            ? MoveComponent->UpdatedComponent->GetCollisionObjectType()
			                                            : ECC_Pawn);
		const FVector TraceStart(Owner->GetActorLocation());
		const FVector TraceEnd(TraceStart.X, TraceStart.Y, (TraceStart.Z - GroundDistance - CapsuleHalfHeight));

		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(LyraCharacterMovementComponent_GetGroundInfo), false, Owner);
		FCollisionResponseParams ResponseParam;
		MoveComponent->InitCollisionParams(QueryParams, ResponseParam);

		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, CollisionChannel, QueryParams,
		                                     ResponseParam);

		LastGroundDistance = GroundTraceDistance;
		if (MoveComponent->MovementMode == MOVE_NavWalking)
		{
			LastGroundDistance = 0.0f;
		}
		else if (HitResult.bBlockingHit)
		{
			LastGroundDistance = FMath::Max((HitResult.Distance - CapsuleHalfHeight), 0.0f);
		}
	}
	LastUpdateFrame = GFrameCounter;
	return LastGroundDistance;
}


void UMyAnimInstance::UpdateLocationData(float DeltaTime)
{
	DisplacementSinceLastUpdate = (PreWorldLocation - WorldLocation).Size2D();
	DisplacementSpeed = UKismetMathLibrary::SafeDivide(DisplacementSinceLastUpdate, DeltaTime);

	if (bIsFirstUpdate)
	{
		DisplacementSinceLastUpdate = 0;
		DisplacementSpeed = 0;
	}
}

void UMyAnimInstance::UpdateRotationData(float DeltaTime)
{
	YawDeltaSinceLastUpdate = WorldRotation.Yaw - PreWorldRotation.Yaw;
	const float YawDeltaSpeed = UKismetMathLibrary::SafeDivide(YawDeltaSinceLastUpdate, DeltaTime);

	AdditiveLeanAngle = YawDeltaSpeed * 0.0375f;

	if (bIsFirstUpdate)
	{
		YawDeltaSinceLastUpdate = 0;
		AdditiveLeanAngle = 0;
	}
}

void UMyAnimInstance::UpdateVelocityData()
{
	bWasMovingLastUpdate = !LocalVelocity2D.IsZero();
	const FVector WorldVelocity2D(WorldVelocity.X, WorldVelocity.Y, 0);
	LocalVelocity2D = WorldRotation.UnrotateVector(WorldVelocity2D);

	LocalVelocityDirectionAngle = UKismetAnimationLibrary::CalculateDirection(WorldVelocity2D, WorldRotation);
	LocalVelocityDirectionAngleWithOffset = LocalVelocityDirectionAngle - RootYawOffset;

	LocalVelocityDirection = SelectCardinalDirectionFromAngle(LocalVelocityDirectionAngleWithOffset,
	                                                          CardinalDirectionDeadZone,
	                                                          LocalVelocityDirection, bWasMovingLastUpdate);

	LocalVelocityDirectionNoOffset = SelectCardinalDirectionFromAngle(LocalVelocityDirectionAngle,
	                                                                  CardinalDirectionDeadZone,
	                                                                  LocalVelocityDirectionNoOffset,
	                                                                  bWasMovingLastUpdate);

	bHasVelocity = !FMath::IsNearlyZero(LocalVelocity2D.SizeSquared2D());
}

void UMyAnimInstance::UpdateAccelerationData()
{
	const FVector WorldAcceleration2D(CurrentAcceleration.X, CurrentAcceleration.Y, 0);
	LocalAcceleration2D = WorldRotation.UnrotateVector(WorldAcceleration2D);
	bHasAcceleration = !FMath::IsNearlyZero(LocalAcceleration2D.SizeSquared2D());

	PivotDirection2D = FMath::Lerp(PivotDirection2D, WorldAcceleration2D.GetSafeNormal(), 0.5f).GetSafeNormal();

	const float Angle = UKismetAnimationLibrary::CalculateDirection(PivotDirection2D, WorldRotation);
	const ECardinalDirection CurrentDirection = SelectCardinalDirectionFromAngle(
		Angle, CardinalDirectionDeadZone, ECardinalDirection::ECD_Forward, false);
	CardinalDirectionFromAcceleration = GetOppositeCardinalDirection(CurrentDirection);
}

void UMyAnimInstance::UpdateRootYawOffset(float InDeltaTime)
{
	switch (RootYawOffsetMode)
	{
	case ERootYawOffsetMode::ERYOM_Accumulate:
		SetRootYawOffset(RootYawOffset - YawDeltaSinceLastUpdate);
		break;
	case ERootYawOffsetMode::ERYOM_BlendOut:
		SetRootYawOffset(
			UKismetMathLibrary::FloatSpringInterp(RootYawOffset, 0, RootYawOffsetSpringState,
			                                      80, 1, InDeltaTime, 1, 0.5));
		break;
	default:
		break;
	}

	RootYawOffsetMode = ERootYawOffsetMode::ERYOM_BlendOut;
}

void UMyAnimInstance::SetRootYawOffset(float InRootYawOffset)
{
	const float NormalizeRootYawOffset = FRotator::NormalizeAxis(InRootYawOffset);
	RootYawOffset = RootYawOffsetAngleClamp.X == RootYawOffsetAngleClamp.Y
		                ? NormalizeRootYawOffset
		                : FMath::ClampAngle(NormalizeRootYawOffset, RootYawOffsetAngleClamp.X,
		                                    RootYawOffsetAngleClamp.Y);
}

TObjectPtr<UAnimSequence> UMyAnimInstance::SelectDirectionalAnimation(const FCardinalDirections& Cardinals,
                                                                      ECardinalDirection Direction)
{
	switch (Direction)
	{
	default:
		// falls through
	case ECardinalDirection::ECD_Forward:
		return Cardinals.Forward;
	case ECardinalDirection::ECD_Backward:
		return Cardinals.Backward;
	case ECardinalDirection::ECD_Left:
		return Cardinals.Left;
	case ECardinalDirection::ECD_Right:
		return Cardinals.Right;
	}
}


ECardinalDirection UMyAnimInstance::SelectCardinalDirectionFromAngle(float Angle, float DeadZone,
                                                                     ECardinalDirection CurrentDirection,
                                                                     bool bUseCurrentDirection)
{
	const float AbsAngle = FMath::Abs(Angle);
	float FwdDeadZone = DeadZone;
	float BwdDeadZone = DeadZone;

	if (bUseCurrentDirection)
	{
		switch (CurrentDirection)
		{
		case ECardinalDirection::ECD_Forward:
			FwdDeadZone *= 2;
			break;
		case ECardinalDirection::ECD_Backward:
			BwdDeadZone *= 2;
			break;
		default:
			break;
		}
	}

	if (AbsAngle <= FwdDeadZone + 45)
	{
		return ECardinalDirection::ECD_Forward;
	}
	else if (AbsAngle >= 135 - BwdDeadZone)
	{
		return ECardinalDirection::ECD_Backward;
	}
	else if (Angle > 0)
	{
		return ECardinalDirection::ECD_Right;
	}
	else
	{
		return ECardinalDirection::ECD_Left;
	}
}

ECardinalDirection UMyAnimInstance::GetOppositeCardinalDirection(ECardinalDirection CurrentDirection)
{
	switch (CurrentDirection)
	{
	default:
		// falls through
	case ECardinalDirection::ECD_Forward:
		return ECardinalDirection::ECD_Backward;
	case ECardinalDirection::ECD_Backward:
		return ECardinalDirection::ECD_Forward;
	case ECardinalDirection::ECD_Left:
		return ECardinalDirection::ECD_Right;
	case ECardinalDirection::ECD_Right:
		return ECardinalDirection::ECD_Left;
	}
}
