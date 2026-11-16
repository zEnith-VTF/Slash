

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Character_Type.h"
#include "Interfaces/Pick_Interface.h"
#include "SlashCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UGroomComponent;
class AItem;
class UAnimMontage;
class USlashOverlay;

struct FInputActionValue;

UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter,public IPick_Interface
{
	GENERATED_BODY()

public:

	ASlashCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
	virtual void SetOverlappingItem(class AItem* Item)override;
	virtual void AddSouls(class ASouls* Souls)override;
	virtual void AddGolds(class ATreasure* Treasure)override;
	
protected:
	
	virtual void BeginPlay() override;
	
	void MoveFunction(const FInputActionValue& Value);
	void LookFunction(const FInputActionValue& Value);
	void JumpPressedFunction(const FInputActionValue& Value);
	void JumpReleasedFunction(const FInputActionValue& Value);
	void PickupFunction(const FInputActionValue& Value);
	void AttackFunction(const FInputActionValue& Value);
	void SwitchFunction(const FInputActionValue& Value);
	void DodgeFunction(const FInputActionValue& Value);
	//以下是动画蒙太奇需要的函数

	void PlayEquipAnimationMontage(UAnimMontage* MontageToPlay, FName SectionName);
	void PlayDeathAnimationMontage();
	void PlayDodgeAnimationMontage();
	virtual void AttackEnd()override;
	UFUNCTION(BlueprintCallable)
	void DodgeEnd();
	virtual bool CanAttack() override;

	UFUNCTION(BlueprintCallable)
	void DisArm();
	UFUNCTION(BlueprintCallable)
	void Arm();
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	virtual void Attacking()override;
	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPos;
private:
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> SlashCharacterMappingContext;
	//初始化HUD数值
	void InitializingSlahOverlay();
	UPROPERTY(Transient)
	UAnimMontage* ActiveDeathMontage;
	virtual void Death() override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;//移动
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;//转视角
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;//跳跃
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> PickupAction;//拾取
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> AttackAction;//攻击
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SwitchAction;//切换武器
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> DodgeAction;//闪避
	UPROPERTY(VisibleAnywhere, category = Hair)
	UGroomComponent* Hair;
	UPROPERTY(VisibleAnywhere, category = Hair)
	UGroomComponent* Eyebrows;
	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;
	
	UPROPERTY(VisibleInstanceOnly)
	ECharacterState CharacterState = ECharacterState::ECS_UnEquipped;
	UPROPERTY(BlueprintReadWrite,meta=(AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;
	UPROPERTY(BlueprintReadWrite,meta=(AllowPrivateAccess = "true"))
	ECharacterWeaponState CharacterWeaponState = ECharacterWeaponState::EIS_InHand;
	//以下是动画蒙太奇需要的变量
	
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* EquipMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage*DodgeMontage;
	UPROPERTY(EditAnywhere)
	float AttackSpeed=2.0f;
	UPROPERTY()
	USlashOverlay* SlashOverlay;



public:
	FORCEINLINE ECharacterState GetCharacterState()const { return CharacterState; }
	FORCEINLINE EActionState GetActionState()const { return ActionState; }
	FORCEINLINE EDeathPose GetDeathPos()const { return DeathPos; }
};
