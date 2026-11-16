// Fill out your copyright notice in the Description page of Project Settings.

// Item为后面的物品类，包含了物品的基本属性和功能，如物品状态、碰撞检测、特效等。它继承自AActor类，可以在游戏世界中被放置和交互。
#pragma once

#include "CoreMinimal.h"//包含了演员通常需要的核心基本功能
#include "GameFramework/Actor.h"
#include "Items/Item_Type.h"
#include "Item.generated.h"//生成器宏，允许Unreal Engine的反射系统识别这个类，并为其生成必要的代码

class USphereComponent;
class UNiagaraComponent;
UCLASS()
class SLASH_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties（构造函数时设置默认值）
	AItem();
	// Called every frame（每帧调用）
    virtual void Tick(float DeltaTime) override;
	EItemState GetItemState()const{ return ItemState; }
	
	
protected:
	// Called when the game starts or when spawned（游戏开始或生成是调用）
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category ="Sine Parameters")
	float Amplitude = 0.35f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Sine Parameters")
	float TimeConstant = 5.f;
	UFUNCTION(BlueprintPure)
	float TransformedSin(float value)const;
	UFUNCTION(BlueprintPure)
	float TransformedCosine()const;
	template<class T>
	T Average(T first ,T second);
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;
	bool bCanSwap();
	//物品状态，默认是悬浮状态
	EItemState ItemState = EItemState::EIS_Hovering;
	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* NiagaraComponent;

	void SpawnPickupEffect();

private: 
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess= "true"))
	float RunningTime = 0.f;
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* PickupEffect;
	
	
};


