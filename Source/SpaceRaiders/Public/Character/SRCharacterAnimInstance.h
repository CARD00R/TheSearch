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

UCLASS()
class SPACERAIDERS_API USRCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Proprties")
	EStanceStatus StanceStatus;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Proprties")
	EInAirStatus InAirStatus;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Proprties")
	EStandingMovementStatus StandingMovementStatus;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Proprties")
	ECrouchingMovementStatus CrouchingMovementStatus;
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MovementSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Proprties")
	float MovementSpeedX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Proprties")
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


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Proprties")
	bool bIsArmed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool SlideRequest;

	

	UFUNCTION(BlueprintCallable, Category = "Animation Properties")
	void UpdateAnimationProperties();

	void DetermineVerticalVelocityProperties();

	
public:

	virtual void NativeInitializeAnimation() override;

	
};
