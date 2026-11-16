// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/Hit_Interface.h"
#include "Components/AttributeComponent.h"
#include "BaseCharacter.generated.h"


class AWeapon;
class UAttributeComponent;
class UAnimMontage;

UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHit_Interface
{
	GENERATED_BODY()

public:
	
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnable);
protected:
	virtual void BeginPlay() override;
	virtual void Attacking();	
	virtual bool CanAttack();
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	UPROPERTY(VisibleAnywhere, category = Weapon)
	AWeapon* EquippedWeapon;
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* AttackMontage; 
	virtual void Death();
	void PlayHitAnimationMontage(const FName& SectionName);

	bool PlayAttackAnimationMontage(UAnimMontage* MontageToPlay,float &AttackSpeed);
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* HitMontage;
	void DirectionalHitReact(const FVector& ImpactPoint);

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* DeathMontage;

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* AttributeComponent;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* HitSound;//不需要前向声明
	UPROPERTY(EditAnywhere, Category = "VisualEffects")
	UParticleSystem* HitParticle;//不需要前向声明

	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	
	virtual void HandleDamage(float DamageAmount);

	bool PlayMontageSection(UAnimMontage* AnimMontage, const FName& SectionName,float& AttackSpeed);
	UPROPERTY(EditAnyWhere, Category = Combat)
	TArray<FName>AttackMontageSection;

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter);

};
