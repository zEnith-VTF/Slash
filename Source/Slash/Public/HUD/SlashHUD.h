// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SlashHUD.generated.h"

/**
 * 
 */
class USlashOverlay;

UCLASS()
class SLASH_API ASlashHUD : public AHUD
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay()override;
private:
	//包含了一个指向USlashOverlay类的指针SlashOverlayClass和一个指向USlashOverlay对象的指针SlashOverlay。
	UPROPERTY(EditDefaultsOnly,Category= Slash) 
	TSubclassOf<USlashOverlay>SlashOverlayClass;
	UPROPERTY()
	USlashOverlay* SlashOverlay;
public:
	//提供获取SlashOverlay指针的函数
	FORCEINLINE USlashOverlay* GetSlashOberlay()const { return SlashOverlay; }
};
