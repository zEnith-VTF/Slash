#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Slash/Debug_macro.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AttributeComponent.h"
#include "Components/WidgetComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Characters/Character_Type.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Items/Souls.h"
AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	//避免Mesh也被设置为Pawn的碰撞通道
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	//允许重叠事件发生 
	GetMesh()->SetGenerateOverlapEvents(true);
	
	HealthBarWidgetComponent = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBarWidgetComponent"));
	HealthBarWidgetComponent->SetupAttachment(GetRootComponent());
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	//设置感知组件的视觉半径
	PawnSensing->SightRadius = 800.f;
	//设置视野角度
	PawnSensing->SetPeripheralVisionAngle(60.f);

	//设置角色朝向与移动方向一致
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
}
 
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	EnemyState = EEnemyState::EES_Patrolling;
	if (AttributeComponent && HealthBarWidgetComponent)
	{
		HealthBarWidgetComponent->SetHealthPercent(AttributeComponent->GetHealthPercent());
		HideHealthBar();
	}
	AIController = Cast<AAIController>(GetController());
	if (PatrolPoint == nullptr)
	{
		//如果没有指定巡逻点，就从巡逻点列表中随机选择一个
		PatrolPoint = ChoosePatrolPoint();
	}
	//这是一个bool类型的函数，但并没有使用他的返回值，代码依旧可以执行
	MoveToTarget(PatrolPoint, PatrolAcceptanceRadius);
	if (PawnSensing)
	{
		//将敌人视野控制在战斗半径内，避免敌人能看到过远的玩家
		PawnSensing->SightRadius = FMath::Min(PawnSensing->SightRadius, static_cast<float>(CombatRadius));
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		if (DefaultWeapon)
		{
			DefaultWeapon->Pickup(GetMesh(), FName(TEXT("Right_Sword")), this, this);
			EquippedWeapon = DefaultWeapon;
		}
	}
	Tags.Add(FName(TEXT("Enemy")));

}
void AEnemy::Attacking()
{
	if (CombatTarget && CombatTarget->ActorHasTag(TEXT("Dead")))
	{
		CombatTarget = nullptr;
		return;
	}
	//if中也会调用PlayAttackAnimationMontage函数
	if (PlayAttackAnimationMontage(AttackMontage,AttackSpeed))
	{
		EnemyState = EEnemyState::EES_Engaged;
		return;
	}
	AttackEnd();
}

void AEnemy::AttackEnd()
{
	if (EnemyState == EEnemyState::EES_Dead) return;
	if (!IsValid(CombatTarget) || IsOutsideCombatRadius())
	{
		LoseInterest();
		return;
	}
	if (IsOutsideAttackRadius())
	{
		ChaseTarget();
		return;
	}
	EnemyState = EEnemyState::EES_Chasing;
	StartAttackTimer();
}


void AEnemy::PlayDeathAnimationMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		ActiveDeathMontage = DuplicateObject<UAnimMontage>(DeathMontage, this);
		if (ActiveDeathMontage == nullptr) return;
		//死亡蒙太奇播到最后一帧后，不自动淡出，不回 Idle，保持死亡最后姿势。
		ActiveDeathMontage->bEnableAutoBlendOut = false;

		AnimInstance->Montage_Play(ActiveDeathMontage, 1.5f);
		int32 Selection = FMath::RandRange(0, 1);
		FName SectionName = FName();
		switch (Selection)
		{
		case 0:
			SectionName = FName(TEXT("Death1"));
			DeathPos = EDeathPose::EDP_Death1;
			break;
		case 1:
			SectionName = FName(TEXT("Death2"));
			DeathPos = EDeathPose::EDP_Death2;
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, ActiveDeathMontage);
	}
}



void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);		
	if (HealthBarWidgetComponent)
	{
		HealthBarWidgetComponent->SetHealthPercent(AttributeComponent->GetHealthPercent());
	}
}

bool AEnemy::EnsureAIController()
{
	//确保AIController有效
	if (AIController == nullptr)
	{
		AIController = Cast<AAIController>(GetController());
	}
	return AIController != nullptr;
}

bool AEnemy::MoveToTarget(AActor* Target, double AcceptanceRadius)
{
	if (!EnsureAIController() || Target == nullptr) return false;
	//构造AI移动请求，创建AI 移动任务的配置包
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	//不必完全到达目标点，设置一个接受半径，为防止非Float类型，将double类型的接受半径转换为float类型
	MoveRequest.SetAcceptanceRadius(static_cast<float>(AcceptanceRadius));
	//创建变量存储这次移动请求有没有成功提交给 AI 路径跟随系统的回答，同时控制ai行动
	const FPathFollowingRequestResult MoveResult = AIController->MoveTo(MoveRequest);
	//MoveResult.Code是AI路径跟随系统对这次移动请求的回答，只有当它不等于失败时才认为移动请求成功提交了
	return MoveResult.Code != EPathFollowingRequestResult::Failed;
}

AActor* AEnemy::ChoosePatrolPoint()
{
	//确保不重复选择当前的巡逻点
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolPoints)
	{
		if (Target && Target != PatrolPoint)
		{
			ValidTargets.Add(Target);
		}
	}
	//从有效的巡逻点中随机选择一个
	if (ValidTargets.Num() > 0)
	{
		int32 Selection = FMath::RandRange(0, ValidTargets.Num() - 1);
		if (ValidTargets[Selection])
		{
			return ValidTargets[Selection];			
		}
	}
	return PatrolPoint;
	
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	if (SeenPawn == nullptr || !SeenPawn->ActorHasTag(FName("SlashCharacter"))) return;
	if (EnemyState == EEnemyState::EES_Dead) return;

	if ((EnemyState == EEnemyState::EES_Attacking || EnemyState == EEnemyState::EES_Engaged) && IsValid(CombatTarget))
	{
		return;
	}

	CombatTarget = SeenPawn;
	if (EnemyState == EEnemyState::EES_Attacking || EnemyState == EEnemyState::EES_Engaged) return;
	//如果正在巡逻，看到玩家后就停止巡逻计时器，直接追击玩家
	ClearPatrolTimer();
	ChaseTarget();
}
void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}
void AEnemy::Death()
{
	if (EnemyState == EEnemyState::EES_Dead) return;

	EnemyState = EEnemyState::EES_Dead;
	ClearAttackTimer();
	ClearPatrolTimer();
	if (AIController)
	{
		AIController->StopMovement();
	}
	PlayDeathAnimationMontage();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	//阻止角色朝向与移动方向一致，保持死亡姿势，防止死亡后角色转向
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetLifeSpan(5.f);
	HideHealthBar();
	SpawnSoul();
}
void AEnemy::SpawnSoul()
{
	UWorld* World = GetWorld();
	if (!World || !SoulClass || !AttributeComponent) return;
	//创建一个生成参数结构体
	FActorSpawnParameters SpawnParams;
	//使用参数结构体设置生成参数，确保生成的灵魂不会因为碰撞而被销毁
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 100.f);
	ASouls* SpawnedSoul = World->SpawnActor<ASouls>(SoulClass, SpawnLocation, GetActorRotation(), SpawnParams);
	if (SpawnedSoul)
	{
		//设置生成的灵魂的属性值
		SpawnedSoul->SetSouls(AttributeComponent->GetSoul());
		SpawnedSoul->SetOwner(this);
	}
}
bool AEnemy::IsOutsideRadius(AActor* Target, double Radius) const
{
	if (!IsValid(Target)) return true;
	//使用Distance函数计算敌人和目标之间的距离，判断是否超过指定的半径
	const double DistanceToTarget = FVector::Distance(Target->GetActorLocation(), GetActorLocation());
	return DistanceToTarget > Radius;
}
bool AEnemy::IsOutsideCombatRadius() const
{
	return IsOutsideRadius(CombatTarget, CombatRadius);
}
bool AEnemy::IsOutsideAttackRadius() const
{
	return IsOutsideRadius(CombatTarget, AttackRadius);
}
void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	if (EnemyState == EEnemyState::EES_Dead) return;
	EnemyState = EEnemyState::EES_Patrolling;
	//恢复巡逻速度
	GetCharacterMovement()->MaxWalkSpeed = 80.f;
	HideHealthBar();
	if (PatrolPoint == nullptr)
	{
		PatrolPoint = ChoosePatrolPoint();
	}
	MoveToTarget(PatrolPoint, PatrolAcceptanceRadius);
}
void AEnemy::ChaseTarget()
{
	if (!IsValid(CombatTarget) || EnemyState == EEnemyState::EES_Dead) return;

	const bool bShouldRestartMove = EnemyState != EEnemyState::EES_Chasing ||
		AIController == nullptr ||
		AIController->GetMoveStatus() != EPathFollowingStatus::Moving;

	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;

	if (bShouldRestartMove)
	{
		MoveToTarget(CombatTarget, 100.f);
	}
}
void AEnemy::AttackTarget()
{
	if (EnemyState == EEnemyState::EES_Dead) return;
	if (!IsValid(CombatTarget) || IsOutsideCombatRadius())
	{
		LoseInterest();
		return;
	}
	if (IsOutsideAttackRadius())
	{
		ChaseTarget();
		return;
	}

	if (AIController)
	{
		AIController->StopMovement();
	}
	GetCharacterMovement()->StopMovementImmediately();
	Attacking();
}
void AEnemy::ShowHealthBar()
{
	if (HealthBarWidgetComponent)
	{
		//设置血条可见
		HealthBarWidgetComponent->SetVisibility(true);
	}
}
void AEnemy::HideHealthBar()
{
	if (HealthBarWidgetComponent)
	{
		//设置血条不可见
		HealthBarWidgetComponent->SetVisibility(false);
	}
}
void AEnemy::PatrolTimerFinished()
{
	//如果当前的巡逻点无效或者敌人不在巡逻点的接受半径内，就选择一个新的巡逻点并移动到那里
	if (!MoveToTarget(PatrolPoint, PatrolAcceptanceRadius))
	{
		PatrolPoint = ChoosePatrolPoint();
		MoveToTarget(PatrolPoint, PatrolAcceptanceRadius);
	}
}

void AEnemy::StartAttackTimer()
{
	if (GetWorldTimerManager().IsTimerActive(AttackTimer)) return;
	if (EnemyState != EEnemyState::EES_Attacking &&
		EnemyState != EEnemyState::EES_Engaged &&
		EnemyState != EEnemyState::EES_Dead)
	{
		EnemyState = EEnemyState::EES_Attacking;
		//设置攻击间隔
		const float MinAttackTime = FMath::Max(0.1f, FMath::Min(AttackMin, AttackMax));
		const float MaxAttackTime = FMath::Max(MinAttackTime, FMath::Max(AttackMin, AttackMax));
		const float AttackTime = FMath::FRandRange(MinAttackTime, MaxAttackTime);
		//获取世界计时器管理器
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::AttackTarget, AttackTime, false);
	}
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(EnemyState == EEnemyState::EES_Dead) return;
	if (EnemyState == EEnemyState::EES_Chasing ||
		EnemyState == EEnemyState::EES_Attacking ||
		EnemyState == EEnemyState::EES_Engaged)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
}

void AEnemy::CheckPatrolTarget()
{
	//如果没有AI控制器，就无法执行巡逻行为，直接返回
	if (!EnsureAIController()) return;

	if (PatrolPoint == nullptr)
	{
		PatrolPoint = ChoosePatrolPoint();
		MoveToTarget(PatrolPoint, PatrolAcceptanceRadius);
		return;
	}
	//如果正在移动，但速度很慢，且不在巡逻点的接受半径内，并且巡逻计时器没有激活，就设置一个新的巡逻计时器，防止AI卡死
	if (AIController->GetMoveStatus() == EPathFollowingStatus::Moving)
	{
		if (GetVelocity().Size2D() <= 1.f && IsOutsideRadius(PatrolPoint, PatrolAcceptanceRadius) && !GetWorldTimerManager().IsTimerActive(PatrolTimer))
		{
			const float RetryTime = FMath::Max(1.f, WaitMin);
			GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, RetryTime, false);
		}
		return;
	}

	if (IsOutsideRadius(PatrolPoint, PatrolAcceptanceRadius))
	{
		if (!MoveToTarget(PatrolPoint, PatrolAcceptanceRadius))
		{
			PatrolPoint = ChoosePatrolPoint();
			MoveToTarget(PatrolPoint, PatrolAcceptanceRadius);
		}
		return;
	}

	if (GetWorldTimerManager().IsTimerActive(PatrolTimer)) return;

	if (AActor* NewPatrolPoint = ChoosePatrolPoint())
	{
		PatrolPoint = NewPatrolPoint;
		const float WaitTime = FMath::FRandRange(WaitMin, WaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime, false);
	}
}
void AEnemy::CheckCombatTarget()
{
	//没有目标，就失去目标
	if (!IsValid(CombatTarget))
	{
		ClearAttackTimer();
		LoseInterest();
		return;
	}
	//如果在战斗半径之外，就失去目标
	if (IsOutsideCombatRadius())
	{
		ClearAttackTimer();
		LoseInterest();
		return;
	}
	//如果在攻击半径之外，就追击目标
	if (IsOutsideAttackRadius())
	{
		ClearAttackTimer();
		ChaseTarget();
		return;
	}

	StartAttackTimer();
}


void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	/*DRAW_SPHERE_SingleFrame(ImpactPoint);*/
	Super::GetHit_Implementation(ImpactPoint,Hitter);
	ShowHealthBar();
	ClearPatrolTimer();
	ClearAttackTimer();

	if (EnemyState == EEnemyState::EES_Dead) return;
	if (!IsValid(CombatTarget) && Hitter)
	{
		//如果没有有效的战斗目标，并且有攻击者，就把攻击者设为战斗目标
		CombatTarget = Hitter;
	}
	if (!IsValid(CombatTarget) || IsOutsideCombatRadius())
	{
		LoseInterest();
		return;
	}
	if (IsOutsideAttackRadius())
	{
		ChaseTarget();
		return;
	}
	EnemyState = EEnemyState::EES_Chasing;
}
void AEnemy::Destroyed()
{
	Super::Destroyed();
	//销毁敌人时，如果装备了武器，就销毁武器
	if(EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}



float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	if (AttributeComponent->GetHealthPercent()==0)
	{
		Death();
		return DamageAmount;
	}
	if (EventInstigator)
	{
		ClearPatrolTimer();
		ClearAttackTimer();
		CombatTarget = EventInstigator->GetPawn();
		ChaseTarget();
	}
	return DamageAmount;
}
