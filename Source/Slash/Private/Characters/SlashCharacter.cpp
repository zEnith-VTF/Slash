// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "EnhancedInputSubsystems.h"//增强输入本地玩家子系统调用需要
#include "EnhancedInputComponent.h"//增强输入连接输入操作需要
#include "InputMappingContext.h"//输入映射需要
#include "InputAction.h"//输入操作需要
#include "InputActionValue.h"//增强输入回调函数参数需要
#include "GameFramework/PlayerController.h" // 获取控制器需要
#include "Engine/LocalPlayer.h"             // 获取本地玩家需要
#include "GameFramework/SpringArmComponent.h"//弹簧臂组件需要
#include "Camera/CameraComponent.h"//摄像机组件需要
#include "GameFramework/CharacterMovementComponent.h"//角色移动组件需要
#include "GroomComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "Components/AttributeComponent.h"
#include "Items/Souls.h"
#include "Items/Treasure.h"

ASlashCharacter::ASlashCharacter()
{	
	PrimaryActorTick.bCanEverTick = true;
	//弹簧臂组件的生成
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 250.f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = false;
	SpringArm->bEnableCameraRotationLag = false;
	//摄像机组件的生成
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);
	ViewCamera->bUsePawnControlRotation = false;
	//毛发组件的生成
	Hair=CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	//附加到的骨骼名称
	Hair->AttachmentName=TEXT("head");
	//眉毛组件的生成
	Eyebrows=CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName=TEXT("head");
	//角色跟随控制器Yaw旋转，也就是跟随鼠标左右转向
	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	//关闭按移动方向自动转身，避免WASD带动角色插值旋转
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	//角色跟随控制器Yaw时，RotationRate不会主导转向
	GetCharacterMovement()->RotationRate = FRotator(0.f, 300.f, 0.f);
	//设置角色网格体的碰撞属性
	GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);
	//先把所有通道的碰撞响应设置为忽略，再针对特定通道设置响应方式
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Block);
	//设置角色网格体对动态物体的碰撞响应为重叠
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	//开启生成重叠事件，这样角色网格体就能触发重叠事件了
	GetMesh()->SetGenerateOverlapEvents(true);
	//设置角色控制器自动接管玩家输入，Player0是默认的第一个玩家
	AutoPossessPlayer = EAutoReceiveInput::Player0;

}

 
void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 强制使用鼠标/控制器Yaw即时控制角色朝向，避免蓝图默认值打开插值转向。
	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
	if (SpringArm)
	{
		SpringArm->bEnableCameraLag = false;
		SpringArm->bEnableCameraRotationLag = false;
	}
	// 1. 获取玩家控制器
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		// 2. 获取增强输入子系统
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// 3. 把控制模式 (IMC) 插进玩家大脑
			if (SlashCharacterMappingContext)
			{
				// 4. 优先级为 0，意味着这个映射上下文会覆盖任何其他映射上下文里相同输入的绑定
				Subsystem->AddMappingContext(SlashCharacterMappingContext, 0);
			}
		}
	}
	// 给角色添加一个标签，方便在蓝图里通过标签来识别角色
	Tags.Add(FName(TEXT("SlashCharacter")));
	//
	InitializingSlahOverlay();
	
}

void ASlashCharacter::InitializingSlahOverlay()
{
	// 1. 获取玩家控制器,因为HUD是玩家控制器的一个组件，所以要先获取玩家控制器才能获取HUD
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		// 2. 获取HUD并转换为ASlashHUD类型
		ASlashHUD* SlashHUD = Cast<ASlashHUD>(PlayerController->GetHUD());
		if (SlashHUD)
		{
			//接管HUD里面的Overlay组件，给它设置初始值
			SlashOverlay = SlashHUD->GetSlashOberlay();
			if (SlashOverlay)
			{
				SlashOverlay->SetHealthPercent(1.f);
				SlashOverlay->SetStaminaPercent(1.f);
				SlashOverlay->SetGold(0);
				SlashOverlay->SetSoul(0);
			}
		}
	}
}

void ASlashCharacter::Death()
{
	if (ActionState == EActionState::EAS_Dead) return;
	//更改标签
	Tags.AddUnique(TEXT("Dead"));
	if (AttributeComponent)
	{
		ActionState = EActionState::EAS_Dead;
		PlayDeathAnimationMontage();
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		//设置角色死亡后5秒钟销毁
		SetLifeSpan(5.f);
	}
}

void ASlashCharacter::MoveFunction(const FInputActionValue& Value)
{
	// 既然是 Axis2D，包裹里装的就是二维向量 FVector2D！直接连锅端！
	FVector2D MovementVector = Value.Get<FVector2D>();

	// 确保角色有灵魂控制
	if (Controller)
	{
		if(ActionState!=EActionState::EAS_Unoccupied) return;
		// 根据控制器的水平朝向计算移动方向，让移动相对镜头，而不是锁死在角色当前朝向。
		const FRotator ControlRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
		const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// 2. 前后推力 (通常 W/S 被映射到了 Y 轴)
		AddMovementInput(Forward, MovementVector.Y);

		// 3. 左右推力 (通常 A/D 被映射到了 X 轴)
		AddMovementInput(Right, MovementVector.X);


	}
}

void ASlashCharacter::LookFunction(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (ActionState == EActionState::EAS_Dead) return;
	if (Controller)
	{
		//乘以系数可以调整鼠标灵敏度
		AddControllerYawInput(LookAxisVector.X*0.5);
		AddControllerPitchInput(LookAxisVector.Y*0.5);
	}
}

void ASlashCharacter::JumpPressedFunction(const FInputActionValue& Value)
{
	//采用内置的跳跃函数
	if (ActionState != EActionState::EAS_Unoccupied) return;
	Jump();
}

void ASlashCharacter::JumpReleasedFunction(const FInputActionValue& Value)
{
	//停止跳跃
	if (ActionState != EActionState::EAS_Unoccupied) return;
	StopJumping();
}

void ASlashCharacter::PickupFunction(const FInputActionValue& Value)
{
	const bool bPressed = Value.Get<bool>();
	//只有当按键被按下并且角色当前没有在执行其他动作时，才允许拾取物品
	if (!bPressed || ActionState != EActionState::EAS_Unoccupied) return;

	AWeapon* Weapon = Cast<AWeapon>(OverlappingItem);
	if (Weapon)
	{
		//传入拾取者和伤害来源
		Weapon->Pickup(GetMesh(), FName(TEXT("RightHandSocket")), this, this);
		CharacterWeaponState = ECharacterWeaponState::EIS_InHand;
		EquippedWeapon = Weapon;
		SetOverlappingItem(nullptr);
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
		//PlayEquipAnimationMontage(EquipMontage, FName(TEXT("UnEquip")));
	}

}

void ASlashCharacter::AttackFunction(const FInputActionValue& Value)
{
	const bool bPressed = Value.Get<bool>();
	
	Attacking();
}

void ASlashCharacter::SwitchFunction(const FInputActionValue& Value)
{
	const bool bPressed = Value.Get<bool>();
	if (!bPressed || ActionState != EActionState::EAS_Unoccupied) return;
	if(CharacterWeaponState == ECharacterWeaponState::EIS_UnHand&&EquippedWeapon&&ActionState==EActionState::EAS_Unoccupied)
	{
		
		PlayEquipAnimationMontage(EquipMontage, FName(TEXT("Equip")));
		CharacterWeaponState = ECharacterWeaponState::EIS_InHand;
		ActionState = EActionState::EAS_EquippingWeapon;
		CharacterState=ECharacterState::ECS_EquippedOneHandedWeapon;

	}
	else if(CharacterWeaponState == ECharacterWeaponState::EIS_InHand && EquippedWeapon && ActionState == EActionState::EAS_Unoccupied)
	{
		PlayEquipAnimationMontage(EquipMontage, FName(TEXT("UnEquip")));
		CharacterWeaponState = ECharacterWeaponState::EIS_UnHand;
		ActionState = EActionState::EAS_EquippingWeapon;
		CharacterState=ECharacterState::ECS_UnEquipped;
	}

}

void ASlashCharacter::DodgeFunction(const FInputActionValue& Value)
{
	const bool bPressed = Value.Get<bool>();
	if (!bPressed || ActionState != EActionState::EAS_Unoccupied || AttributeComponent == nullptr) return;
	//确保角色有足够的耐力来执行翻滚动作
	if (AttributeComponent->GetStamina() < AttributeComponent->GetDodgeCost()) return;

	PlayDodgeAnimationMontage();
	ActionState = EActionState::EAS_Dodge;
	AttributeComponent->UseStamina(AttributeComponent->GetDodgeCost());
	if (SlashOverlay)
	{
		SlashOverlay->SetStaminaPercent(AttributeComponent->GetStaminaPercent());
	}
}



void ASlashCharacter::PlayEquipAnimationMontage(UAnimMontage* MontageToPlay, FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && MontageToPlay)
	{
		AnimInstance->Montage_Play(MontageToPlay, 1.5f);
		//选择需要播放的动画段落
		AnimInstance->Montage_JumpToSection(SectionName, MontageToPlay);
	}


}
void ASlashCharacter::PlayDeathAnimationMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		//创建一个新的动画蒙太奇实例来播放死亡动画，如果直接更改，会导致所有使用同一蒙太奇的角色都播放死亡动画
		ActiveDeathMontage = DuplicateObject<UAnimMontage>(DeathMontage, this);
		if (ActiveDeathMontage == nullptr) return;
		AnimInstance->Montage_Play(ActiveDeathMontage, 1.0f);
		//随机选择一个死亡动画段落来播放，使用RandRange函数。
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
		//防止动画自动淡出，这样角色就不会在死亡动画播放完后立刻站起来
		ActiveDeathMontage->bEnableAutoBlendOut = false;
	}
}

void ASlashCharacter::PlayDodgeAnimationMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DodgeMontage)
	{		
		AnimInstance->Montage_Play(DodgeMontage, 2.0f);
	}

}
//AttackEnd函数会在蓝图中被调用
void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}
//DodgeEnd函数会在蓝图中被调用
void ASlashCharacter::DodgeEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::CanAttack()
{
	const bool bCanAttack =
		ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_UnEquipped;
	return bCanAttack;
}
//取消装备武器时，把武器附加到角色的背部
void ASlashCharacter::DisArm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName(TEXT("SpineSocket")));
	}
}
//装备武器时，把武器附加到角色的手上
void ASlashCharacter::Arm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName(TEXT("RightHandSocket")));
	}
}

void ASlashCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::Attacking()
{
	if (CanAttack() && PlayAttackAnimationMontage(AttackMontage,AttackSpeed))
	{
		ActionState = EActionState::EAS_Attacking;
	}
}

void ASlashCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}


void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//设置角色每秒钟恢复耐力
	if(AttributeComponent)
	{
		AttributeComponent->StaminaRegeneration(DeltaTime);
		if (SlashOverlay)
		{
			SlashOverlay->SetStaminaPercent(AttributeComponent->GetStaminaPercent());
		}
	}

}


void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 2. 告诉大管家：按下()时，触发 () 函数！
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASlashCharacter::MoveFunction);
		}
		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::LookFunction);

		}
		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ASlashCharacter::JumpPressedFunction);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ASlashCharacter::JumpReleasedFunction);
		}
		if(PickupAction)
		{
			EnhancedInputComponent->BindAction(PickupAction, ETriggerEvent::Started, this, &ASlashCharacter::PickupFunction);
		}
		if (AttackAction)
		{
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ASlashCharacter::AttackFunction);
		}
		if (SwitchAction)
		{
			EnhancedInputComponent->BindAction(SwitchAction, ETriggerEvent::Started, this, &ASlashCharacter::SwitchFunction);
		}
		if (DodgeAction)
		{
			EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &ASlashCharacter::DodgeFunction);
		}
}


}
//特殊规则,GetHit函数需要写成GetHit_Implementation。
void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint,Hitter);
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	if (AttributeComponent && AttributeComponent->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}
}

float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//通过调用父类的TakeDamage函数来处理伤害
	HandleDamage(DamageAmount);
	if (SlashOverlay&&AttributeComponent)
	{
		SlashOverlay->SetHealthPercent(AttributeComponent->GetHealthPercent());
	}
	if (AttributeComponent->GetHealthPercent()==0)
	{
		Death();
		return DamageAmount;
	}
	return DamageAmount;
}
//重写Destroyed函数，在角色被销毁时销毁它装备的武器
void ASlashCharacter::Destroyed()
{
	Super::Destroyed();
	
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}
//有意思
void ASlashCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void ASlashCharacter::AddSouls(ASouls* Souls)
{
	if (AttributeComponent)
	{
		AttributeComponent->AddSouls(Souls->GetSouls());
		if (SlashOverlay)
		{
			SlashOverlay->SetSoul(AttributeComponent->GetSoul());
		}
	}
}

void ASlashCharacter::AddGolds(ATreasure* Treasure)
{
	if (AttributeComponent)
	{
		AttributeComponent->AddGold(Treasure->GetGold());
		if (SlashOverlay)
		{
			SlashOverlay->SetGold(AttributeComponent->GetGold());
		}
	}
}


