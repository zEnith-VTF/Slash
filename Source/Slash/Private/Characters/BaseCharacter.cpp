// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/Character_Type.h"
#include "Animation/AnimMontage.h"

ABaseCharacter::ABaseCharacter()
{	
	PrimaryActorTick.bCanEverTick = true;
	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Attacking()
{
	
}
bool ABaseCharacter::CanAttack()
{
	return false;

}
void ABaseCharacter::AttackEnd()
{


}
//统一播放受击动画的接口
void ABaseCharacter::PlayHitAnimationMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitMontage)
	{
		AnimInstance->Montage_Play(HitMontage, 1.5f);
		AnimInstance->Montage_JumpToSection(SectionName, HitMontage);
	}
}
bool ABaseCharacter::PlayAttackAnimationMontage(UAnimMontage* MontageToPlay, float &AttackSpeed)
{
	//AttackMontageSection是一个FName类型的数组，在蓝图中设置完成
	if (AttackMontageSection.Num() <= 0 || MontageToPlay == nullptr) return false;
	const int32 Section = FMath::RandRange(0, AttackMontageSection.Num() - 1);
	return PlayMontageSection(MontageToPlay, AttackMontageSection[Section],AttackSpeed);
}
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnable)
{
	if (EquippedWeapon && EquippedWeapon->GetBoxComponents())
	{
		EquippedWeapon->GetBoxComponents()->SetCollisionEnabled(CollisionEnable);
		EquippedWeapon->IgnoreActors.Empty();
	}
}
void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	const FVector LowerImpactPoint = FVector(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (LowerImpactPoint - GetActorLocation()).GetSafeNormal();
	//求点积
	const double CosTheta = FMath::Clamp(FVector::DotProduct(Forward, ToHit), -1.0, 1.0);
	double Theta = FMath::Acos(CosTheta);//结果是弧度，需要转换成角度
	Theta = FMath::RadiansToDegrees(Theta);

	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta = -Theta;
	}
	FName Section = TEXT("FromFront");
	if (Theta >= -60.f && Theta <= 60.f)
	{
		Section = FName("FromBack");
	}
	else if (Theta > -135.f && Theta < -60.f)
	{
		Section = FName("FromLeft");
	}
	else if (Theta <= 135.f && Theta >= 60.f)
	{
		Section = FName("FromRight");
	}

	PlayHitAnimationMontage(Section);

	
}
void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}
}
void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint)
{
	if (HitParticle && GetWorld())
	{
		//UGameplayStatics::SpawnEmitterAtLocation()函数用于在指定位置生成粒子效果。
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticle,
			ImpactPoint);
	}
}
void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (AttributeComponent)
	{
		AttributeComponent->ReceiveDamage(DamageAmount);
	}
}
bool ABaseCharacter::PlayMontageSection(UAnimMontage* AnimMontage, const FName& SectionName, float &AttackSpeed)
{

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AnimMontage && !SectionName.IsNone())
	{
		//当传进来的SectionName在AnimMontage中不存在时，GetSectionIndex()函数会返回INDEX_NONE
		if (AnimMontage->GetSectionIndex(SectionName) == INDEX_NONE) return false;
		//限制最小速度
		const float PlayRate = FMath::Max(AttackSpeed, 0.1f);
		const float MontageLength = AnimInstance->Montage_Play(AnimMontage, PlayRate);
		if (MontageLength <= 0.f) return false;
		AnimInstance->Montage_JumpToSection(SectionName, AnimMontage);
		return true;
	}
	return false;
}
void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	const bool bIsAlive = AttributeComponent == nullptr || AttributeComponent->IsAlive();
	if (bIsAlive && Hitter)
	{
		DirectionalHitReact(Hitter->GetActorLocation());
	}
	else Death();
	PlayHitSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);

}
void ABaseCharacter::Death()
{

}


