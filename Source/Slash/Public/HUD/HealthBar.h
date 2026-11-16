// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BluePrint/UserWidget.h"
#include "HealthBar.generated.h"

/**
 * 
 */
class UProgressBar;

UCLASS()
class SLASH_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()
public:
	//绑定进度条组件
	UPROPERTY(meta=(BindWidget))
	UProgressBar* HealthBar;

};
