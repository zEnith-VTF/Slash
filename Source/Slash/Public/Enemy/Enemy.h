// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Characters/BaseCharacter.h"
#include "Characters/Character_Type.h"
#include "Enemy.generated.h"

class UAttributeComponent;
class UHealthBarComponent;
class AAIController;
class UPawnSensingComponent;
class AWeapon;
class UAnimMontage;

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	
	// GetHit_Implementation是接口函数GetHit的实现函数，接口函数GetHit会调用GetHit_Implementation；如果在蓝图中重写了GetHit函数，那么GetHit_Implementation就不会被调用。
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void Destroyed() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void CheckPatrolTarget();
	void CheckCombatTarget();

protected:
	virtual void BeginPlay() override;
	virtual void Attacking() override;
	virtual void AttackEnd() override;

	// 状态数据
	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPos;

	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;
	virtual void HandleDamage(float DamageAmount)override;
	// AI移动与感知回调
	UFUNCTION()
	bool MoveToTarget(AActor* Target, double AcceptanceRadius);
	AActor* ChoosePatrolPoint();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

	void ClearPatrolTimer();

	UPROPERTY(BlueprintReadOnly, Category = Combat)
	AActor* CombatTarget;
private:
	virtual void Death() override;
	void SpawnSoul();
	bool EnsureAIController();
	bool IsOutsideRadius(AActor* Target, double Radius) const;
	bool IsOutsideCombatRadius() const;
	bool IsOutsideAttackRadius() const;
	void LoseInterest();
	void ChaseTarget();
	void AttackTarget();
	void ShowHealthBar();
	void HideHealthBar();

	UPROPERTY(Transient)
	UAnimMontage* ActiveDeathMontage;

	UFUNCTION()
	void PlayDeathAnimationMontage();
	void PatrolTimerFinished();

	// 组件引用
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidgetComponent;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	// 战斗目标与检测范围
	void StartAttackTimer();
	void ClearAttackTimer();

	FTimerHandle  AttackTimer;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMin = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMax = 1.5f;
	

	UPROPERTY(EditAnywhere)
	double CombatRadius = 800.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 250.f;

	UPROPERTY(EditAnywhere)
	float AttackSpeed = 1.0f;
	// 巡逻目标与到达判定
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	double PatrolAcceptanceRadius = 200.f;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolPoint;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolPoints;

	// AI控制器
	UPROPERTY()
	AAIController* AIController;

	// 巡逻等待计时
	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMin = 2.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMax = 5.f;

	// 武器生成配置
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> WeaponClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ASouls>SoulClass;
};