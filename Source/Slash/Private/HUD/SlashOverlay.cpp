// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
void USlashOverlay::SetHealthPercent(float Percent)
{
	if (HealthBar)
	{
		//调用内部函数设置血条百分比
		HealthBar->SetPercent(Percent);
	}
	
}

void USlashOverlay::SetStaminaPercent(float Percent)
{
	if (StaminaBar)
	{
		StaminaBar->SetPercent(Percent);
	}

}

void USlashOverlay::SetGold(int32 Gold)
{
	if (GoldText)
	{	
		//Printf转字符串，因为 UMG 里的 TextBlock->SetText() 需要的是FText类型，所以需要先把整数转成字符串，再把字符串转成文本
		GoldText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Gold)));
	}
}

void USlashOverlay::SetSoul(int32 Soul)
{
	if (SoulText)
	{
		
		SoulText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Soul)));
	}
}
