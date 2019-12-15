// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Character/SRCharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "public/Character/SRCharacter.h"

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
		FTransform MeshTransform = GetSkelMeshComponent()->GetComponentTransform();

		// Obtain horizontal Speed (Inversed [-1])
		MovementSpeedX = (UKismetMathLibrary::InverseTransformDirection(MeshTransform, Speed).X)*-1.0f;
		// Obtain Vertical Speed
		MovementSpeedY = UKismetMathLibrary::InverseTransformDirection(MeshTransform, Speed).Y;
		// Obtain Directional Speed
		MovementSpeed = LateralSpeed.Size();
	
		bIsInAir = Pawn->GetMovementComponent()->IsFalling();
			
		//Calculate Direction 
		Direction = CalculateDirection(Speed, Pawn->GetActorRotation());
		Direction = FMath::Clamp(Direction, -175.0f, 175.0f);

		// Casting to SRCharacter in order to obtain the Movement Status Enum inside ASR Character
		Character = Cast<ASRCharacter>(Pawn);
		
		if(Character != nullptr)
		{
			MovementStatus = Character->GetStandingMovementStatus();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Character Cast Failed"));

			Character = Cast<ASRCharacter>(Pawn);
		}
		

	}
	else
	{
		Pawn = TryGetPawnOwner();
	}	
}
