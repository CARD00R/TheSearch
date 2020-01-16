// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SRHealthComponent.generated.h"

//On Health changed event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, USRHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(SpaceRaiders), meta=(BlueprintSpawnableComponent) )
class SPACERAIDERS_API USRHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USRHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Health Component")
	float CurrentHealth;
	UPROPERTY(BlueprintReadOnly, Category = "Health Component")
	float MaxHealth;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Health Component")
	float StartingHealth;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:
	
	UPROPERTY(BlueprintAssignable, Category= "Events")
	FOnHealthChangedSignature OnHealthChanged;
		
};
