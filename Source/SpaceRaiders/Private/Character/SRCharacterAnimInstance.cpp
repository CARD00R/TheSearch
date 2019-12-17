// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Character/SRCharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "public/Character/SRCharacter.h"
#include "TimerManager.h"

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

		// Fly/Fall Speed
		VerticalVelocity = Pawn->GetVelocity().Z;
		
		// Obtain horizontal Speed (Inversed [-1])
		MovementSpeedX = (UKismetMathLibrary::InverseTransformDirection(MeshTransform, Speed).X)*-1.0f;
		// Obtain Vertical Speed
		MovementSpeedY = UKismetMathLibrary::InverseTransformDirection(MeshTransform, Speed).Y;
		// Obtain Directional Speed
		MovementSpeed = LateralSpeed.Size();
	
		//Calculate Direction 
		Direction = CalculateDirection(Speed, Pawn->GetActorRotation());
		Direction = FMath::Clamp(Direction, -175.0f, 175.0f);

		// Casting to SRCharacter in order to obtain the Movement Status Enum inside ASR Character
		Character = Cast<ASRCharacter>(Pawn);
				
		if(Character != nullptr)
		{
			StanceStatus = Character->GetStanceStatus();
			InAirStatus = Character->GetInAirStatus();
			bIsArmed = Character->GetIsArmed();
			
			DetermineVerticalVelocityProperties();
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

void USRCharacterAnimInstance::DetermineVerticalVelocityProperties()
{
	StoredZLocation = Character->GetActorLocation().Z;
	
	if (VerticalVelocity >= 0)
	{	
		if (VerticalVelocity > 0)
		{
			Character->SetInAirStatus(EInAirStatus::Eias_Jumping);
		}
		if(VerticalVelocity == 0)
		{	
			// Call Timer
			if(bShouldResetFallHeight)
			{
				GetWorld()->GetTimerManager().SetTimer(TimerFallHeightReset, this, &USRCharacterAnimInstance::ResetFallHeight, 1.2f, false);
				bShouldResetFallHeight = false;
			}
		}
		
		FallHeightStartingZ = StoredZLocation;
	}
	else
	{
		
		
		if(FallHeight > FallHeight)
		{
			Character->SetInAirStatus(EInAirStatus::Eias_Flailing);		
		}
		else
		{
			Character->SetInAirStatus(EInAirStatus::Eias_Falling);
			bShouldResetFallHeight = true;
		}

		FallHeight = FallHeightStartingZ - StoredZLocation;
	}
}

void USRCharacterAnimInstance::ResetFallHeight()
{
	FallHeight = 0;
}
