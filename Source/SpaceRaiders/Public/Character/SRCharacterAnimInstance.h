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

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MovementSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Proprties")
	float MovementSpeedX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Proprties")
	float MovementSpeedY;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	APawn* Pawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	ASRCharacter* Character;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Proprties")
	EStanceStatus StanceStatus;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Proprties")
	bool bIsArmed;

	UFUNCTION(BlueprintCallable, Category = "Animation Properties")
	void UpdateAnimationProperties();


	
public:

	virtual void NativeInitializeAnimation() override;
	
};
