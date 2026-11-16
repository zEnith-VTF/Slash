// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashAnimInstance.h"
#include "Characters/SlashCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
void USlashAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	SlashCharacter = Cast<ASlashCharacter>(TryGetPawnOwner());
	if(SlashCharacter)
	{
		SlashCharacterMovement = SlashCharacter->GetCharacterMovement();
	}
}

void USlashAnimInstance::NativeUpdateAnimation(float DeltaTimes)
{
	Super::NativeUpdateAnimation(DeltaTimes);
	if (SlashCharacter == nullptr)
	{
		//如果没有角色，就重新获取一次，避免角色在游戏中被销毁后动画实例失效
		SlashCharacter = Cast<ASlashCharacter>(TryGetPawnOwner());
	}
	if (SlashCharacter == nullptr) return;

	SlashCharacterMovement = SlashCharacter->GetCharacterMovement();
	if (SlashCharacterMovement)
	{
		//获取角色的地面速度，使用VSizeXY函数只计算水平速度，忽略垂直速度
		//需要包含KismetMathLibrary.h头文件才能使用VSizeXY函数
		GroundSpeed = UKismetMathLibrary::VSizeXY(SlashCharacterMovement->Velocity);
		//使用角色移动组件的IsFalling函数来判断角色是否在空中，如果在空中则为true，否则为false
		IsFalling = SlashCharacterMovement->IsFalling();
		CharacterState = SlashCharacter->GetCharacterState();
		ActionState = SlashCharacter->GetActionState();
		DeathPose = SlashCharacter->GetDeathPos();
	}
}