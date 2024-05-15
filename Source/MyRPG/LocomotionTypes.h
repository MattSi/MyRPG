#pragma once
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "LocomotionTypes.generated.h"

UENUM()
enum class ECardinalDirection:uint8
{
	ECD_Forward,
	ECD_Backward,
	ECD_Left,
	ECD_Right
};

UENUM()
enum class ERootYawOffsetMode :uint8
{
	ERYOM_BlendOut,
	ERYOM_Hold,
	ERYOM_Accumulate
};


USTRUCT(BlueprintType)
struct FCardinalDirections
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UAnimSequence> Forward;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UAnimSequence> Backward;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UAnimSequence> Left;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UAnimSequence> Right;
};