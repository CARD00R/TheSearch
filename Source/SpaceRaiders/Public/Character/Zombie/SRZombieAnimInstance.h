// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SRZombie.h"
#include "SRZombieAnimInstance.generated.h"

/**
 * 
 */

class APawn;
class ASRZombie;

UCLASS()
class SPACERAIDERS_API USRZombieAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Properties|Status")
	EZStanceStatus StanceStatus;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Properties|Status")
	EZStandingStatus StandingStatus;
protected:
	UFUNCTION(BlueprintCallable, Category = "Animation Properties")
	void UpdateAnimationProperties(float DeltaTime);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	APawn* Pawn;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	ASRZombie* Character;

	// Movement Variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Properties")
	float Direction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Properties")
	float MovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Properties")
	float MovementSpeedX;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement Properties")
	float MovementSpeedY;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Falling Properties")
	float VerticalVelocity;
	
public:

	virtual void NativeInitializeAnimation() override;
};
