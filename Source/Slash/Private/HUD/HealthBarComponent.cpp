// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarComponent.h"
#include "HUD/HealthBar.h"
#include "Components/ProgressBar.h"
void UHealthBarComponent::SetHealthPercent(float Percent)
{
	
	if (HealthBarWidget==nullptr)
	{
		//获取这个 WidgetComponent 当前创建出来的 UserWidget 实例
		HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());
		
	}
	
	if (HealthBarWidget&&HealthBarWidget->HealthBar)
	{
		//设置进度条
		HealthBarWidget->HealthBar->SetPercent(Percent);
		
	}
}
