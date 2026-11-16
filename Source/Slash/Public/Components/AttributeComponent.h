// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void StaminaRegeneration(float DeltaTime);
	bool IsAlive();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health = 100.f;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth = 100.f;	
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Stamina = 100.f;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxStamina = 100.f;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float DodgeCost = 20.f;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaRegenRate = 5.f;
	UPROPERTY(VisibleAnywhere, Category = "Actor Attributes")
	float Golds = 0;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Souls = 0;
	int32 IfDeath = false;

	
public:
	void ReceiveDamage(float Damage);
	void UseStamina(float StaminaCost);
	float GetHealthPercent();
	float GetStaminaPercent();
	void AddSouls(int32 SoulAmount);
	void AddGold(int32 GoldAmount);
	FORCEINLINE int32 GetGold()const { return Golds; }
	FORCEINLINE int32 GetSoul()const { return Souls; }
	FORCEINLINE float GetDodgeCost()const { return DodgeCost; }
	FORCEINLINE float GetStamina()const { return Stamina; }
};
