// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"
#include "Interfaces/Pick_Interface.h"
#include "Kismet/GameplayStatics.h"

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPick_Interface*Pick_Interface= Cast<IPick_Interface>(OtherActor);
	if (Pick_Interface)
	{
		Pick_Interface->AddGolds(this);
		if (PickupSound) 
		{
			UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
		}
		Destroy();//Ïú»Ù±¦²Ø´úÂë
	}
}
