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
class USphereComponent;
class USoundBase;
class UTexture2D;


UENUM(BlueprintType)
enum class EGunType : uint8
{
	Egt_Pistol UMETA(DisplayName = "Pistol"),
	Egt_Shotgun UMETA(DisplayName = "Shotgun"),
	Egt_SMG UMETA(DisplayName = "SMG"),
	Egt_AssaultRifle UMETA(DisplayName = "AssaultRifle"),
	Egt_Sniper UMETA(DisplayName = "Sniper"),
	Egt_Max UMETA(DisplayName = "DefaultMax")
};

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* PickUpCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* StencilCollision;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UTexture2D* WeaponPortrait;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UCameraShake> FireCameraShake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	USoundBase* FireSFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	USoundBase* HitSFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	USoundBase* ReloadSFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	USoundBase* EmptyMagSFX;
	
	void PlayFireEffects(FVector TracerEnd, FHitResult HitRes);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	EGunType GunType;
	
	// Values
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BaseDamage = 20.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	// RPM - Bullets per minute fired
	float RateOfFire;
	void Fire();
	float LastFiredTime;
	float TimeBetweenShots;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Recoil")
	float HorizontalRecoil;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Recoil")
	float VerticalRecoil;

	
	FTimerHandle FireTimer;
public:
	
	void StartFire();
	
	void StopFire();

	void Reload();

	void ReloadStart();

	void ReloadEnd();

	

	//States
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Weapon")
	bool IsPickedUp;
	// Montages
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UAnimMontage* StandingFireMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UAnimMontage* CrouchingFireMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UAnimMontage* ReloadMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UAnimMontage* PickUpGunMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UAnimMontage* PickUpAmmoMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UAnimMontage* HolsterMontage;


	void PickedupCollisionPreset();
	void SetPickedUpState(bool NewIsPickedUp);
	void DroppedCollisionPreset();
	void PlayPickUpGunMontage();
	void PlayPickUpAmmoMontage();
	void PlayHolsterMontage();

	
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//variables
		//Bullet Spread in Degrees
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = 0.0))
		float BulletSpread;
	//Reload
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		float MagSize;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		float CurrentBulletsInMag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		float BulletsInReserve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		float BulletForce;

};
