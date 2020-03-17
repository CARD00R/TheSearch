// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SRCharacter.h"
#include "SRPickup.generated.h"


class USphereComponent;

UENUM(BlueprintType)
enum class EPickupType : uint8
{
	Ept_Ammo UMETA(DisplayName = "Ammo"),
	Ept_Health UMETA(DisplayName = "Health"),
	Ept_Stamina UMETA(DisplayName = "Stamina"),
	Ept_Currency UMETA(DisplayName = "Currency"),
	Ept_Max UMETA(DisplayName = "DefaultMax")
};
UCLASS()
class SPACERAIDERS_API ASRPickup : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASRPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	EPickupType PickupType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* PickupCollision;



public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	EPickupType GetUtilityPickupType();
	void ConsumeUtility();
	void Store();
};