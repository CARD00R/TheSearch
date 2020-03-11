// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SRCharacter.h"
#include "SRCharacterAnimInstance.generated.h"


/**
 * 
 */

class APawn;
class ASRCharacter;
class ASRGun;

UCLASS()
class SPACERAIDERS_API USRCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Properties")
	EStanceStatus StanceStatus;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Properties")
	EInAirStatus InAirStatus;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Properties")
	EStandingMovementStatus StandingMovementStatus;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Properties")
	ECrouchingMovementStatus CrouchingMovementStatus;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Properties")
	EGunStatus GunStatus;
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Properties")
	float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Properties")
	float MovementSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Properties")
	float MovementSpeedX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Properties")
	float MovementSpeedY;
	
	# pragma region Falling Properties
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Falling Properties")
	float VerticalVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Falling Properties")
	float FallHeightFlailLimit = 700;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Falling Properties")
	float FallHeight;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Falling Properties")
	float StoredFallHeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Falling Properties")
	float FallHeightStartingZ;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Falling Properties")
	bool bShouldHardLand;

	bool bShouldResetFallHeight = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aiming Properties")
	float Pitch;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aiming Properties")
	float Yaw;
	#pragma endregion

	//Timers
	FTimerHandle TimerFallHeightReset;
	void ResetFallHeight();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float StoredZLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	APawn* Pawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	ASRCharacter* Character;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Properties")
	bool bGunHolstered;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool SlideRequest;


	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void ReloadBullets();
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void ReloadEnd();
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void PullingOutPistol();
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void HolsteringPistol();
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void PickingUpPistol();
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void HolsterSecondaryPistol();
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void HolsterPrimaryPistol();

	UFUNCTION(BlueprintCallable, Category = "Animation Properties")
	void UpdateAnimationProperties(float DeltaTime);

	void DetermineVerticalVelocityProperties();

	void CalculateYawPitch(float DeltaTime);
public:

	virtual void NativeInitializeAnimation() override;


	
};
