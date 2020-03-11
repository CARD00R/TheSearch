// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SRGun.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class UCameraShake;
class UAnimMontage;
class ASRCharacter;

UCLASS()
class SPACERAIDERS_API ASRGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASRGun();

protected:
	
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	// Effects
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* MuzzleEffect;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* SelectedImpactEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* CharImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* CharCritImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* CharHeadImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* MetalImpactEffect;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* TracerEffect;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UCameraShake> FireCameraShake;
	
	void PlayFireEffects(FVector TracerEnd, FHitResult HitRes);

	//States
	UPROPERTY(VisibleAnwywhere, Category = "Weapon")
	bool IsPickedUp;

	// Values
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BaseDamage = 20.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	// RPM - Bullets per minute fired
	float RateOfFire;
	void Fire();
	float LastFiredTime;
	float TimeBetweenShots;

	//Reload
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float MagSize;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float CurrentBulletsInMag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float BulletsInReserve;

	
	
	FTimerHandle FireTimer;
public:
	
	void StartFire();
	
	void StopFire();

	void Reload();

	void ReloadStart();

	void ReloadEnd();

	
	// Montages
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UAnimMontage* FireMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UAnimMontage* ReloadMontage;
};
