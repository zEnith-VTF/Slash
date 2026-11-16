// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

/**
 * 
 */
class USoundBase;
class UBoxComponent;
class IHit_Interface;

UCLASS()
class SLASH_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();
	void Pickup(USceneComponent*InParent,FName SocketName,AActor *NewOwner,APawn * NewInstigator);
	void PlayEquipSound();
	void AttachMeshToSocket(USceneComponent* InParent, const FName& SocketName);
	UPROPERTY()
	TArray<AActor*>IgnoreActors;




protected:	
	virtual void BeginPlay()override;
	
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent*OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	bool ActorIsSameType(AActor* WeaponOwner, AActor* OtherActor);
	void ExecuteGetHit(IHit_Interface* HitInterface, FHitResult& BoxHit);
	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector BoxTraceExtent = FVector(1.5f);
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowBoxDebug = false;


	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	bool bEnableHitField = false;
	//声音变量，采用USoundBase类型，允许在编辑器中设置武器拾取时的声音效果，仅定义，未实装
	UPROPERTY(EditAnywhere,Category="Weapon Properties")
	USoundBase* EquipSound;

	UPROPERTY(VisibleAnywhere,Category="Weapon Properties")
	UBoxComponent* WeaponBox;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;
	UPROPERTY(EditAnywhere,Category="Weapon Properties")
	float Damage = 20.f;
	
public:
	FORCEINLINE UBoxComponent* GetBoxComponents()const { return WeaponBox; };

};
