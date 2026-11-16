// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Characters/Character_Type.h"
#include "SlashAnimInstance.generated.h"
 
class ASlashCharacter;
class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class SLASH_API USlashAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTimes) override;

	UPROPERTY(BlueprintReadOnly)
	ASlashCharacter* SlashCharacter;

	UPROPERTY(BlueprintReadOnly,Category = Movement)
	UCharacterMovementComponent* SlashCharacterMovement;
	
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed;
	UPROPERTY(BlueprintReadOnly)
	bool IsFalling;
	UPROPERTY(BlueprintReadOnly,Category="Movement | CharacterState")
	ECharacterState  CharacterState;
	UPROPERTY(BlueprintReadOnly, Category = "ActionState")
	EActionState ActionState;
	UPROPERTY(BlueprintReadOnly, Category = "ActionState")
	EDeathPose DeathPose;
};
