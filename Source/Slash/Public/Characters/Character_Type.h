#pragma once
UENUM(BlueprintType)//将该枚举加入反射系统并允许在蓝图中作为类型
enum class ECharacterState :uint8
{
	//UMETA(DisplayName = "Death1") 是给蓝图/编辑器看的显示名字。
	ECS_UnEquipped UMETA(DisplayName = "UnEquipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped-OneHanded-Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped-TwoHanded-Weapon")
};
//需要转为uint8类型以节省内存空间
UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_HitReaction UMETA(DisplayName = "HitReaction"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_EquippingWeapon UMETA(DisplayName = "Equipping"),
	EAS_Dodge UMETA(DisplayName = "Dodge"),
	EAS_Dead UMETA(DisplayName = "Dead")
};
UENUM(BlueprintType)
enum class ECharacterWeaponState :uint8
{
	EIS_InHand UMETA(DisplayName = "Inhand"),
	EIS_UnHand UMETA(DisplayName = "Unhand")
};
UENUM(BlueprintType)
enum class EDeathPose : uint8
{
	EDP_Death1 UMETA(DisplayName = "Death1"),
	EDP_Death2 UMETA(DisplayName = "Death2")
};
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
	EES_Engaged UMETA(DisplayName = "Engaged"),
	EES_Dead UMETA(DisplayName = "Dead")
};
