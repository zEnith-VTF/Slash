#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Hit_Interface.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;
class ATreasure;
class UCapsuleComponent;

UCLASS()
class SLASH_API ABreakableActor : public AActor, public IHit_Interface
{
	GENERATED_BODY()

public:
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	UFUNCTION()
	void ReActor(const FChaosBreakEvent& BreakEvent);
protected:
	virtual void BeginPlay() override;
	//ÆÆ»·×é¼þ
	UPROPERTY(VisibleAnywhere)
	UGeometryCollectionComponent* GeometryCollection;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	UCapsuleComponent* CapsuleComponent;

private:
	

	UPROPERTY(EditAnywhere)

	TArray<TSubclassOf<ATreasure>>TreasureClasses;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABreakableActor>BreakableActor;
	bool bBroken = false;

	bool bReActor= false;
};
