// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Character/SRCharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/UnrealMathUtility.h"

void USRCharacterAnimInstance::NativeInitializeAnimation()
{
	if(!Pawn)
	{
		Pawn = TryGetPawnOwner();
	}	
}


void USRCharacterAnimInstance::UpdateAnimationProperties()
{
	if(Pawn)
	{
		//Calculate Movement Speed
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0);
		MovementSpeed = LateralSpeed.Size();

		bIsInAir = Pawn->GetMovementComponent()->IsFalling();
		
		//Calculate Direction 
		Direction = CalculateDirection(Speed, Pawn->GetActorRotation());
		Direction = FMath::Clamp(Direction, -175.0f, 175.0f);
	}
	else
	{
		Pawn = TryGetPawnOwner();
	}	
}
