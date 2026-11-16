// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Treasure.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API ATreasure : public AItem
{
	GENERATED_BODY()
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)override;

private:
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* PickupSound;//不需要前向声明
	UPROPERTY(EditAnywhere, Category = "Treasure Properties")
	int32 Gold = 0;
public:
	//内联函数
	FORCEINLINE int32 GetGold()const { return Gold; }
};
