#pragma once


UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedKatanaWeapon UMETA(DiaplayName = "Equipped Katana Weapon"),
	ECS_EquippedGreatSword UMETA(DisplayName = "Equipped Great Sword")
};

UENUM(BlueprintType)
enum class EActionState :uint8
{
	EAS_Idle UMETA(DisplayName = "IDLE"),
	EAS_HitReaction UMETA(DisplayName = "HitReaction"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_SP_Attacking UMETA(DisplayName = "SP Attacking"),
	EAS_Defence UMETA(DisplayName = "Defence"),
	EAS_EquippingWeapon UMETA(DisplayName = "Equipping Weapon"),
	EAS_Dodge UMETA(DisplayName = "Dodge"),
	EAS_Dead UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class EDeathPose:uint8
{
	EDP_Alive UMETA(DisplayName = "Alive"),
	EDP_DP1 UMETA(DisplayName = "Death Pose 1"),
	EDP_DP2 UMETA(DisplayName = "Death Pose 2"),
	EDP_DP3 UMETA(DisplayName = "Death Pose 3"),
	EDP_DP4 UMETA(DisplayName = "Death Pose 4"),
	EDP_DP5 UMETA(DisplayName = "Death Pose 5"),
	EDP_DP6 UMETA(DisplayName = "Death Pose 6")
};