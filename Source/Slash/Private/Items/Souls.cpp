// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Souls.h"
#include "Interfaces/Pick_Interface.h"

void ASouls::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPick_Interface* Pick_Interface = Cast<IPick_Interface>(OtherActor);
	if (Pick_Interface)
	{
		//Ìí¼ÓÁé»ê
		Pick_Interface->AddSouls(this);
	}
	
	SpawnPickupEffect();
	Destroy();
}
