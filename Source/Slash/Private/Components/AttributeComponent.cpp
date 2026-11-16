


#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;

	
}


void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();


	
}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	//Clamp函数将Health - Damage的结果限制在0和MaxHealth之间，确保Health不会超过MaxHealth或降到0以下。
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	if (Health == 0)
	{
		IfDeath = true;
	}
}

void UAttributeComponent::UseStamina(float StaminaCost)
{
	Stamina = FMath::Clamp(Stamina - StaminaCost, 0.f, MaxStamina);	
}

float UAttributeComponent::GetHealthPercent()
{
	//如果MaxHealth非常小（接近于零），返回0以避免除以零的错误。KINDA_SMALL_NUMBER是一个非常小的常量，通常用于比较浮点数是否接近于零。
	if (MaxHealth <= KINDA_SMALL_NUMBER)
	{
		return 0.f;
	}
	return Health / MaxHealth;

}

float UAttributeComponent::GetStaminaPercent()
{
	if (MaxStamina <= KINDA_SMALL_NUMBER)
	{
		return 0.f;
	}
	return Stamina / MaxStamina;
}

void UAttributeComponent::AddSouls(int32 SoulAmount)
{
	Souls += SoulAmount;
}

void UAttributeComponent::AddGold(int32 GoldAmount)
{
	Golds += GoldAmount;
}



void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	

}
void UAttributeComponent::StaminaRegeneration(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0.f, MaxStamina);
}
bool UAttributeComponent::IsAlive()
{
	return Health > 0.f;
}

