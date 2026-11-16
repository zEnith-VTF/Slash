


#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Items/Treasure.h"
#include "Components/CapsuleComponent.h"
#include <ctime>
ABreakableActor::ABreakableActor()
{
 	
	PrimaryActorTick.bCanEverTick = false;
	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->SetupAttachment(GetRootComponent());
	CapsuleComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

}


void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
	GeometryCollection->OnChaosBreakEvent.AddDynamic(this,&ABreakableActor::ReActor);
}


void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void ABreakableActor::ReActor(const FChaosBreakEvent& BreakEvent)
{
	(void)BreakEvent;
	if (!bBroken || bReActor) return;

	bReActor = true;

	UWorld* World = GetWorld();
	if (!(World && BreakableActor)) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 10.f);
	World->SpawnActor<ABreakableActor>(BreakableActor, SpawnLocation, GetActorRotation(), SpawnParams);
}
void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	//防止重复击中
	if (bBroken) return;
	bBroken = true;

	// 先让几何体开始模拟并主动触发碎裂
	if (GeometryCollection)
	{
		//让几何体开始模拟物理
		GeometryCollection->SetSimulatePhysics(true);
		//给几何集合施加“应变/伤害”。
		GeometryCollection->ApplyExternalStrain(0, ImpactPoint, 100.f, 1, 1.f, 50000.f);
		
		//让已激活簇立即解体，让效果更明显
		GeometryCollection->CrumbleActiveClusters();
	}

	if (CapsuleComponent)
	{
		CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	UWorld* World = GetWorld();
	if (World && TreasureClasses.Num() > 0)
	{
		FVector Location = GetActorLocation();
		Location.Z += 50.f;

		const int32 Selection = FMath::RandRange(0, TreasureClasses.Num() - 1);

		if (TreasureClasses[Selection])
		{
			FActorSpawnParameters SpawnParams;
			//生成时先调整，如果有碰撞就调整位置，但无论如何都生成
			SpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			World->SpawnActor<ATreasure>(
				TreasureClasses[Selection],
				Location,
				GetActorRotation(),
				SpawnParams
			);
		}
	}
}

