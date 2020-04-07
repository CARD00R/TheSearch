// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Character/Zombie/SRZombieAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "UnitConversion.h"


void USRZombieAnimInstance::NativeInitializeAnimation()
{
	if (!Pawn)
	{
		Pawn = TryGetPawnOwner();
	}
}

void USRZombieAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (Pawn)
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
		//MovementSpeed = LateralSpeed.Size();

		//Calculate Direction 
		//Direction = CalculateDirection(Speed, Pawn->GetActorRotation());
		//Direction = FMath::Clamp(Direction, -175.0f, 175.0f);

		// Casting to SRCharacter in order to obtain the Movement Status Enum inside ASR Character
		Character = Cast<ASRZombie>(Pawn);

		// If character exists...		
		if (Character != nullptr)
		{
			//Set numerous variables
			//Status
			StanceStatus = Character->GetStanceStatus();
			StandingStatus = Character->StandingStatus;
			//UE_LOG(LogTemp, Error, TEXT("Status: ", %f));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Character Cast Failed"));

			Character = Cast<ASRZombie>(Pawn);
		}
	/*
		if(StanceStatus == EZStanceStatus::Ezss_Standing)
		{
			if (MovementSpeedY > 10)
			{
				Character->SetStandingStatus(EZStandingStatus::Ezss_Running);
			}
			else
			{
				Character->SetStandingStatus(EZStandingStatus::Ezss_Idling);
			}
		}
		else if (StanceStatus == EZStanceStatus::Ezss_Crawling)
		{
			Character->SetStandingStatus(EZStandingStatus::Ezss_NIS);
			//Set Crawl Speed
		}
		else if (StanceStatus == EZStanceStatus::Ezss_Eating)
		{
			Character->SetStandingStatus(EZStandingStatus::Ezss_NIS);
		}
	*/
	}
	else
	{
		Pawn = TryGetPawnOwner();
	}

}