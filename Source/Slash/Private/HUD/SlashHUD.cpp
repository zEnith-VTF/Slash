// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "Engine/World.h"
void ASlashHUD::BeginPlay()
{
	
	Super::BeginPlay();
	UWorld* World = GetWorld();
	if (World&&SlashOverlayClass)
	{
		//获取第一个玩家控制器
		APlayerController* Controller = World->GetFirstPlayerController();
		//填入玩家控制器和控件类
		SlashOverlay=CreateWidget<USlashOverlay>(Controller,SlashOverlayClass);
		//将控件添加到视口
		SlashOverlay->AddToViewport();
	}
	
}
