// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Characters/SlashCharacter.h"
#include "Kismet/GameplayStatics.h"//游戏玩法静态函数库需要，游戏常用功能。
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"//提供了许多实用的函数，包括各种类型的碰撞检测函数
#include "Interfaces/Hit_Interface.h"
#include "Breakable/BreakableActor.h"
#include "NiagaraComponent.h"
#include "GameFramework/Pawn.h"//Instigator属于APawn类，所以要包含这个头文件
AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox"));
	WeaponBox->SetupAttachment(GetRootComponent());
	//设置碰撞已启用为无碰撞
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//设置所有通道的碰撞响应为重叠，这样当角色进入武器的碰撞范围时，就会触发重叠事件
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	//设置单独通道的响应为忽略
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Overlap);
	//盒体追踪的起点和终点组件
	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("BoxTraceStart"));
	BoxTraceStart->SetupAttachment(GetRootComponent());
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("BoxTraceEnd"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	if (WeaponBox)
	{
		WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
	}

}
void AWeapon::Pickup(USceneComponent* InParent, FName SocketName, AActor* NewOwner, APawn* NewInstigator)
{
	//设置Owner，为了后续能够正确识别拥有者。
	SetOwner(NewOwner);
	//设置发起者，发起者通常是造成伤害的角色，这里我们把它设置为拾取武器的角色
	SetInstigator(NewInstigator);
	AttachMeshToSocket(InParent, SocketName);
	ItemState = EItemState::EIS_Equipped;
	PlayEquipSound();
	if (SphereComponent)
	{
		//解决拾取武器后与人物重叠的问题
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	}
	if (ItemMesh)
	{
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if(NiagaraComponent)
	{
		NiagaraComponent->Deactivate();
	}
}
void AWeapon::PlayEquipSound()
{
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());

	}
}
void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& SocketName)
{
	if (ItemMesh == nullptr || InParent == nullptr) return;
	//新建附加规则
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	//将武器的网格组件附加到父组件上，并指定附加规则和插槽名称
	ItemMesh->AttachToComponent(InParent, AttachmentRules, SocketName);

}



void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	AActor* WeaponOwner = GetOwner();
	if (BoxTraceStart == nullptr || BoxTraceEnd == nullptr) return;
	const FVector Start = BoxTraceStart->GetComponentLocation();//在蓝图中是GetWorldLocation
	const FVector End = BoxTraceEnd->GetComponentLocation();
	//忽略的演员列表，武器本身和角色都要忽略，否则会检测到自己和角色
	TArray<AActor*> IgnoredActors;//类似于Vetcor的容器数组
	IgnoredActors.Add(this);//忽略武器本身
	if (WeaponOwner)
	{
		//AddUnique函数会检查元素是否已经存在于数组中,防止重复添加相同的元素。
		IgnoredActors.AddUnique(WeaponOwner);
	}
	//Instigator属于APawn类
	APawn* InstigatorPawn = GetInstigator();
	if (InstigatorPawn)
	{
		//虽然目前onwer和instigator通常是同一个角色，但是如果存在例如火球等远程武器，instigator可能是发射火球的角色，而owner可能是法杖，所以我们都要忽略。
		IgnoredActors.AddUnique(InstigatorPawn);
	}
	//后续如果有其他需要忽略的演员，例如已经被击中的角色，可以继续添加到这个列表中。
	for (AActor* Actor : IgnoreActors)
	{
		IgnoredActors.AddUnique(Actor);//忽略角色
	}
	FHitResult BoxHit;//这是一个结构体，输出变量，存储检测到的碰撞信息，我们在函数参数中声明了它，函数执行后会被填充
	//开始实施盒体追踪
	UKismetSystemLibrary::BoxTraceSingle(
		this,//World Context Object告诉函数在哪个世界中执行，通常是当前对象
		Start,
		End,
		BoxTraceExtent,//控制范围大小
		FRotator(0, 0, 0),//停止旋转
		ETraceTypeQuery::TraceTypeQuery1,//TraceTypeQuery1相当于TraceTypeQuery(Visibily)
		false,//是否使用复杂碰撞
		IgnoredActors,//忽略演员列表
		bShowBoxDebug?EDrawDebugTrace::ForDuration: EDrawDebugTrace::None,//是否显示调试线条，我们定义为false
		BoxHit,
		true
	);
	if (BoxHit.GetActor())
	{
		//防止同一个角色被多次击中，击中后将角色添加到忽略列表中
		IgnoreActors.AddUnique(BoxHit.GetActor());
		//判断是否击中自己或者同类型的角色，如果是则直接返回，不进行伤害处理
		if (ActorIsSameType(WeaponOwner, BoxHit.GetActor()))
		{
			return;
		}
		//将击中的角色转换为Hit_Interface接口类型，使用接口可以让不同类型的角色都能被武器击中并处理伤害，而不需要关心具体的类继承关系，只要实现了接口就行。
		IHit_Interface* HitInterface = Cast<IHit_Interface>(BoxHit.GetActor());
		if (HitInterface == nullptr) return;
		//调用接口函数
		ExecuteGetHit(HitInterface, BoxHit);
		//查看是否可以获取到伤害造成者的控制器
		AController* InstigatorController = InstigatorPawn ? InstigatorPawn->GetController() : nullptr;
		UGameplayStatics::ApplyDamage(
			BoxHit.GetActor(),//被击中的角色
			Damage,//伤害数值
			InstigatorController,//伤害造成者的控制器
			this,//伤害来源，这里是武器
			UDamageType::StaticClass()//伤害类型，这里使用默认的伤害类型
			);
	}
}

bool AWeapon::ActorIsSameType(AActor* WeaponOwner, AActor* OtherActor)
{
	//避免敌人攻击命中敌人，也就是简单的阵营过滤。
	return WeaponOwner && WeaponOwner->ActorHasTag(FName(TEXT("Enemy"))) && OtherActor && OtherActor->ActorHasTag(FName(TEXT("Enemy")));
}

void AWeapon::ExecuteGetHit(IHit_Interface* HitInterface, FHitResult& BoxHit)
{
	//舍弃版本HitInterface->GetHit(BoxHit.ImpactPoint);
	HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint,GetOwner());

	if (bEnableHitField)
	{
		CreateFields(BoxHit.ImpactPoint);
	}
}
