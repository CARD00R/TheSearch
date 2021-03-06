// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Character/SRCharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "public/Character/SRCharacter.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/Guns/SRGun.h"
#include "Public/Components/SRHealthComponent.h"
#include "Kismet/GameplayStatics.h"

void USRCharacterAnimInstance::NativeInitializeAnimation()
{
	if(!Pawn)
	{
		Pawn = TryGetPawnOwner();
	}	
}


void USRCharacterAnimInstance::UpdateAnimationProperties(float DeltaTime)
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

		// If character exists...		
		if(Character != nullptr)
		{
			//Set numerous variables
			//Status
			StanceStatus = Character->GetStanceStatus();
			InAirStatus = Character->GetInAirStatus();
			StandingMovementStatus = Character->GetStandingMovementStatus();
			CrouchingMovementStatus = Character->GetCrouchingMovementStatus();
			GunStatus = Character->GetGunStatus();

			bShouldRollLand = Character->GetShouldRollLand();
			bGunHolstered = Character->GetGunHolstered();
			bShouldHardLand = Character->GetShouldHardLand();
			Character->FallHeight = FallHeight;
			SlideRequest = Character->SlideRequest;
			
			CalculateYawPitch(DeltaTime);
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
			ResetFallHeight();
		}
		if(VerticalVelocity == 0)
		{	
			// Call Timer
			if(bShouldResetFallHeight)
			{
				GetWorld()->GetTimerManager().SetTimer(TimerFallHeightReset, this, &USRCharacterAnimInstance::ResetFallHeight, 1.7f, false);
				bShouldResetFallHeight = false;
			}
		}
		
		FallHeightStartingZ = StoredZLocation;
	}
	else
	{
		Character->bCheckCapsuleProperties = true;
		Character->SetStanceStatus(EStanceStatus::Ess_InAir);
		
		if(FallHeight > FallHeightFlailLimit)
		{
			Character->SetInAirStatus(EInAirStatus::Eias_Flailing);
			Character->SetShouldRollLand(false);
		}
		else if (FallHeight > FallHeightMiniLandLimit && FallHeight < 1700)
		{
			//bShouldRollLand = true;
			Character->SetShouldRollLand(true);
		}
		else
		{
			Character->SetInAirStatus(EInAirStatus::Eias_Falling);
			Character->SetShouldRollLand(false);
			bShouldResetFallHeight = true;

		}
		FallHeight = FallHeightStartingZ - StoredZLocation;
	}
}

void USRCharacterAnimInstance::CalculateYawPitch(float DeltaTime)
{
	//Current Rotation
	FRotator Current = FRotator(Pitch, Yaw, 0);
	//The Difference between the control rotation(mouse) and the actor rotation
	FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(Character->GetControlRotation(), Character->GetActorRotation());
	// Constantly reseting the value of the return so it can keep up with mouse movement
	FRotator Return = UKismetMathLibrary::RInterpTo(Current, Delta, DeltaTime, 60);
	// Clamping values to prevent unwanted rotation
	Pitch = FMath::ClampAngle(Return.Pitch, -60, 60);
	Yaw = FMath::ClampAngle(Return.Yaw, -60, 60);
	// Setting character yaw/pitch
	Character->Pitch = Pitch;
	Character->Yaw = Yaw;
}

void USRCharacterAnimInstance::ResetFallHeight()
{
	FallHeight = 0;
}

void USRCharacterAnimInstance::ReloadBullets()
{
	Character->EquippedWeapon->Reload();
	APawn* MyPawn = Cast<APawn>(Character);
	APlayerController* PC = Cast<APlayerController>(MyPawn->GetController());
	if (PC)
	{
		PC->ClientPlayCameraShake(Character->ReloadCameraShake);
	}
}

void USRCharacterAnimInstance::ReloadEnd()
{
	Character->EquippedWeapon->ReloadEnd();
}

void USRCharacterAnimInstance::PullingOutPistol()
{
	Character->AnimNotifyUnHolster();
}
void USRCharacterAnimInstance::HolsteringPistol()
{
	Character->AnimNotifyHolster();
}
void USRCharacterAnimInstance::PickingUpPistol()
{
	Character->AnimNotifyPickUpGun();
}

void USRCharacterAnimInstance::HolsterSecondaryPistol()
{
	Character->AnimNotifyHolsterSecondaryGun();
}
void USRCharacterAnimInstance::HolsterPrimaryPistol()
{
	Character->AnimNotifyHolsterPrimaryGun();
}

void USRCharacterAnimInstance::HealCharacter()
{
	if(Character)
	{
		if(Character->OwningHealthComp)
		{
			Character->OwningHealthComp->SetHealth(Character->HealthPackAddition);
			Character->HealthUtilityCount--;
			APawn* MyPawn = Cast<APawn>(Character);
			APlayerController* PC = Cast<APlayerController>(MyPawn->GetController());
			if (PC)
			{
				PC->ClientPlayCameraShake(Character->ReloadCameraShake);
				PC->ClientPlayCameraShake(Character->HealCameraShake);
				//Heal
				UGameplayStatics::PlaySound2D(this, Character->Heal,0.7f);
			}
		}

	}
}
