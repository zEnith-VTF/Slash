// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "Slash/Debug_macro.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "Interfaces/Pick_Interface.h"
#include "NiagaraFunctionLibrary.h"
// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	RootComponent=ItemMesh;

	SphereComponent=CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(GetRootComponent());

	NiagaraComponent=CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();//调用父类的BeginPlay()函数，确保任何在父类中定义的初始化逻辑都能正确执行
	//UWorld* World = GetWorld();
	//SetActorLocation(FVector(0, 0, 200.f));
	//SetActorRotation(FRotator(45.f, 0, 0));
	//FVector ForwardVector = GetActorForwardVector();
	//FVector Location = GetActorLocation();
	//DRAW_LINE(Location,Location+ForwardVector*100.f);
	//DRAW_SPHERE(Location);
	//DRAW_POINT(Location);
	//DRAW_VECTOR(Location, Location + ForwardVector * 100.f);
	//DRAW_RECTANGLE(Location, ForwardVector * 100.f);
	
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this,&AItem::OnSphereOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this,&AItem::OnSphereOverlapEnd);
}

float AItem::TransformedSin(float value)const
{
	return FMath::Sin(TimeConstant * value)* Amplitude;
}
float AItem::TransformedCosine()const

{
	return FMath::Cos(TimeConstant * RunningTime) * Amplitude;
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPick_Interface* Pick_Interface = Cast<IPick_Interface>(OtherActor);
	if (Pick_Interface)
	{
		Pick_Interface->SetOverlappingItem(this);
	}
}

void AItem::OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IPick_Interface* Pick_Interface = Cast<IPick_Interface>(OtherActor);
	if (Pick_Interface)
	{
		Pick_Interface->SetOverlappingItem(nullptr);
	}
	
}

bool AItem::bCanSwap()
{
	return ItemState==EItemState::EIS_Hovering;
}

void AItem::SpawnPickupEffect()
{
	if (PickupEffect)
	{
		//特效组，在位置处生成特效
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PickupEffect, GetActorLocation());
	}
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime+= DeltaTime;
	//float DeltaZ = TransfromedSin(RunningTime);
	//AddActorWorldOffset(FVector(0, 0,DeltaZ));
	//AddActorWorldOffset(FVector(0, 0, MoveSpeed*DeltaTime));
	//AddActorWorldRotation(FRotator(0, RotationSpeed, 0));
	/*DRAW_SPHERE_SingleFrame(GetActorLocation());
	DRAW_VECTOR_SingleFrame(GetActorLocation(), GetActorLocation()+GetActorForwardVector() * 100.f);*/
	/*FVector AvgVector = Average<FVector>(GetActorLocation(),FVector::ZeroVector);
	DRAW_POINT_SingleFrame(AvgVector);*/
	if (bCanSwap())
	{
		AddActorWorldOffset(FVector(0, 0, TransformedSin(RunningTime)));
		AddActorWorldRotation(FRotator(0, TransformedCosine(), 0));
	}
	
}

