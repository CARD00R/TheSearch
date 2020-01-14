// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Character/SRCharacter.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"

// Sets default values
ASRCharacter::ASRCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Mesh
	GetMesh()->SetWorldLocationAndRotation(MeshInitialiseLocation, MeshInitialiseRotation);

	//SpringArmComp
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->SetWorldLocation(SpringArmInitialiseLocation);
	SpringArmComp->SocketOffset = SpringArmInitialiseSocketOffset;
	
	//CameraComp
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	//CapsuleComp
	GetCapsuleComponent()->SetCapsuleRadius(20.0f);
	
	//Movement Component
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanJump = true;
	GetCharacterMovement()->GravityScale = 1.5f;
	GetCharacterMovement()->CrouchedHalfHeight = CrouchingCapsuleHeight;
	GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
	GetCharacterMovement()->BrakingDecelerationWalking = 1350.0f;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->JumpZVelocity = 560.0f;
	

	// Input Properties
	bIsMovingForward = false;
	bIsMovingRight = false;

	// Replication Variables
    SetReplicateMovement(true);
	SetReplicates(true);

	// Default Stance, Movement Statuses
	SetStanceStatus(EStanceStatus::Ess_Standing);
	SetStandingMovementStatus(EStandingMovementStatus::Esms_Idling);
	SetCrouchingMovementStatus(ECrouchingMovementStatus::Ecms_Nis);
	SetInAirStatus(EInAirStatus::Eias_Nis);

	//Global
	bGlobalKeysInput = true;
	bGlobalMouseInput = true;	
}

// Called when the game starts or when spawned
void ASRCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if(StanceStatus == EStanceStatus::Ess_Sliding)
	{
		SlideSlopeDetection();
		SlideSpeedCalculation();
	}
	else if(SlideCheck && StanceStatus != EStanceStatus::Ess_Sliding && InAirStatus != EInAirStatus::Eias_Nis)
	{
		SlideSlopeDetection();
	}
	if(bCheckCapsuleProperties)
	{
		CheckCapsuleHeightRadius();
	}
	if(bChangeFOV)
	{
		SetCameraFOV(DeltaTime);
	}
}

// Called to bind functionality to input
void ASRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//Axis
	PlayerInputComponent->BindAxis("MoveForward", this, &ASRCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASRCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ASRCharacter::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &ASRCharacter::Turn);
	//Action
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASRCharacter::CrouchSlideCheckPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASRCharacter::CrouchSlideCheckReleased);
	PlayerInputComponent->BindAction("FreeLook", IE_Pressed, this, &ASRCharacter::FreeLookOn);
	PlayerInputComponent->BindAction("FreeLook", IE_Released, this, &ASRCharacter::FreeLookOff);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASRCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASRCharacter::SprintReleased);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASRCharacter::StartJump);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ASRCharacter::AimPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ASRCharacter::AimReleased);
	PlayerInputComponent->BindAction("PrimaryWeapon", IE_Pressed, this, &ASRCharacter::EquipPrimaryWeapon);
}

void ASRCharacter::Landed(const FHitResult & Hit)
{
	Super::Landed(Hit);
	// Land and the character is Flailing
	if(InAirStatus == EInAirStatus::Eias_Flailing)
	{
		if(SlideRequest)
		{
			if(SlideStatus != ESlideStatus::Ess_FlatSlope || GetCharacterMovementSpeed() > 1650)
			{
				SetStandingMovementStatus(EStandingMovementStatus::Esms_Nis);
				SetCrouchingMovementStatus(ECrouchingMovementStatus::Ecms_Nis);
				StartSlide();
			}
			else
			{
				SlideRequest = false;
				SetStanceStatus(EStanceStatus::Ess_Standing);
				
				GlobalKeysInputDisable();
				GlobalMouseInputDisable();
				if (FallHeight > 1300.0f)
				{
					LandDelay = HardLandDelay;
					bShouldHardLand = true;
				}
				else
				{
					LandDelay = SoftLandDelay;
					bShouldHardLand = false;
				}
				GetWorld()->GetTimerManager().SetTimer(TimerGlobalKeysInput, this, &ASRCharacter::GlobalKeysInputEnable, LandDelay, false);
				GetWorld()->GetTimerManager().SetTimer(TimerGlobalMouseInput, this, &ASRCharacter::GlobalMouseInputEnable, LandDelay, false);
			}
		}
		else
		{
			SlideRequest = false;
			SetStanceStatus(EStanceStatus::Ess_Standing);
			GlobalKeysInputDisable();
			GlobalMouseInputDisable();
			if (FallHeight > 1300.0f)
			{
				LandDelay = HardLandDelay;
				bShouldHardLand = true;
			}
			else
			{
				LandDelay = SoftLandDelay;
				bShouldHardLand = false;
			}
			GetWorld()->GetTimerManager().SetTimer(TimerGlobalKeysInput, this, &ASRCharacter::GlobalKeysInputEnable, LandDelay, false);
			GetWorld()->GetTimerManager().SetTimer(TimerGlobalMouseInput, this, &ASRCharacter::GlobalMouseInputEnable, LandDelay, false);
		}
	}
	else
	{
		if (!SlideRequest)
		{
			SetStanceStatus(EStanceStatus::Ess_Standing);

			if (StandingMovementStatus == EStandingMovementStatus::Esms_Jogging || StandingMovementStatus == EStandingMovementStatus::Esms_Idling
				|| StandingMovementStatus == EStandingMovementStatus::Esms_Sprinting)
			{
				SetStanceStatus(EStanceStatus::Ess_Standing);
			}
			else if (CrouchingMovementStatus == ECrouchingMovementStatus::Ecms_Idling || CrouchingMovementStatus == ECrouchingMovementStatus::Ecms_Walking)
			{
				SetStanceStatus(EStanceStatus::Ess_Crouching);
			}
		}
		else
		{
			if(SlideStatus != ESlideStatus::Ess_FlatSlope)
			{
				StartSlide();
				SetStandingMovementStatus(EStandingMovementStatus::Esms_Nis);
				SetCrouchingMovementStatus(ECrouchingMovementStatus::Ecms_Nis);
				
			}
			else
			{
				SetStanceStatus(EStanceStatus::Ess_Standing);

				if (StandingMovementStatus == EStandingMovementStatus::Esms_Jogging || StandingMovementStatus == EStandingMovementStatus::Esms_Idling
					|| StandingMovementStatus == EStandingMovementStatus::Esms_Sprinting)
				{
					SetStanceStatus(EStanceStatus::Ess_Standing);
				}
				else if (CrouchingMovementStatus == ECrouchingMovementStatus::Ecms_Idling || CrouchingMovementStatus == ECrouchingMovementStatus::Ecms_Walking)
				{
					SetStanceStatus(EStanceStatus::Ess_Crouching);
				}
			}

			

		}

	}

	SetInAirStatus(EInAirStatus::Eias_Nis);
}

FVector ASRCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}
	
	return Super::GetPawnViewLocation();
}

void ASRCharacter::MoveForward(float value)
{
	
	if(bGlobalKeysInput)
	{
		AddMovementInput(GetActorForwardVector() * value);
		
		//If player is pressing W/S...
		if (value != 0)
		{
			//...then player is moving forward
			bIsMovingForward = true;

			//...and player is standing...
			if (StanceStatus == EStanceStatus::Ess_Standing)
			{	
				// and Player is just pressing S...
				if (value < 0)
				{
					SetCharacterMovementSpeed(BackwardsJogSpeed);
					
					//GetCharacterMovement()->MaxWalkSpeed = BackwardsJogSpeed;
					//...and is sprinting...
					if (StandingMovementStatus == EStandingMovementStatus::Esms_Sprinting)
					{
						//then stop the player from sprinting [you can't spring backwards]
						SetStandingMovementStatus(EStandingMovementStatus::Esms_Jogging);
					}
					else
					{
						SetStandingMovementStatus(EStandingMovementStatus::Esms_Jogging);
					}
				}

				// if the player is just pressing W...
				else
				{
					if(!SlideRequest)
					{
						//...and player is ADSing...
						if(GunStatus == EGunStatus::Egs_ADSing)
						{
							//...Set Speed to walkspeed
							SetCharacterMovementSpeed(WalkSpeed);
						}
						else
						{
							//...and player is moving forward and sprinting...
							if (StandingMovementStatus == EStandingMovementStatus::Esms_Sprinting)
							{
								if (bIsMovingRight)
								{
									SprintSpeed = DiagonalSprintSpeed;
									SetStandingMovementStatus(EStandingMovementStatus::Esms_Sprinting);
									SetCharacterMovementSpeed(SprintSpeed);
								}
								else
								{
									SprintSpeed = DefaultSprintSpeed;
									SetStandingMovementStatus(EStandingMovementStatus::Esms_Sprinting);
									if (GetCharacterMovementSpeed() < DefaultSprintSpeed)
									{
										//SetCharacterMovementSpeed(SprintSpeed);
										SetCharacterMovementSpeed(FMath::Lerp(GetCharacterMovementSpeed(), SprintSpeed, 0.5));
									}
									else if (GetCharacterMovementSpeed() > DefaultSprintSpeed)
									{
										SetCharacterMovementSpeed(FMath::Lerp(GetCharacterMovementSpeed(), SprintSpeed, 0.3));
									}
									else
									{

									}

								}
							}
							//...and player is moving forward and not sprinting...
							else
							{
								//...then player is jogging
								SetStandingMovementStatus(EStandingMovementStatus::Esms_Jogging);

								if (GetCharacterMovementSpeed() <= JogSpeed)
								{
									SetCharacterMovementSpeed(JogSpeed);
								}
								else
								{
									SetCharacterMovementSpeed(FMath::Lerp(GetCharacterMovementSpeed(), JogSpeed, 0.3));
								}
							}
						}
					}	
				}
			}
			//...and player is crouching
			else if (StanceStatus == EStanceStatus::Ess_Crouching)
			{
				SetCrouchingMovementStatus(ECrouchingMovementStatus::Ecms_Walking);
			}
		}
		//If Player is not pressing W/S...
		else
		{
			//...then player is not moving forward
			bIsMovingForward = false;

			if(StanceStatus == EStanceStatus::Ess_Sliding)
			{
				EndSlide();
			}
		}
	}
	
	//If Player is not moving forward and not moving right...
	if(!bIsMovingForward && !bIsMovingRight)
	{
		//...whilst standing
		if(StanceStatus == EStanceStatus::Ess_Standing)
		{//...then player is standing idling
			SetStandingMovementStatus(EStandingMovementStatus::Esms_Idling);
		}
		//...whilst crouching
		else if(StanceStatus == EStanceStatus::Ess_Crouching)
		{
			//...then player is crouching idling
			SetCrouchingMovementStatus(ECrouchingMovementStatus::Ecms_Idling);
		}
	}
	//If player is not moving forward but is moving right...
	else if(!bIsMovingForward && bIsMovingRight)
	{
		//...whilst standing...
		if (StanceStatus == EStanceStatus::Ess_Standing)
		{
			//...then the player is jogging
			SetStandingMovementStatus(EStandingMovementStatus::Esms_Jogging);
		}
		//...whilst crouching...
		else if (StanceStatus == EStanceStatus::Ess_Crouching)
		{
			//...then player is crouching walking
			SetCrouchingMovementStatus(ECrouchingMovementStatus::Ecms_Walking);
		}
	}

}

void ASRCharacter::MoveRight(float value)
{
	if (bGlobalKeysInput)
	{
		AddMovementInput(GetActorRightVector()* value);
		
		//If player is pressing A/D...
		if (value != 0)
		{
			//...then player is moving right
			bIsMovingRight = true;

			//...and player is ADSing...
			if (GunStatus == EGunStatus::Egs_ADSing)
			{
				//...Set Speed to walkspeed
				SetCharacterMovementSpeed(WalkSpeed);
			}
			else
			{
				if (StanceStatus == EStanceStatus::Ess_Standing)
				{
					//...if player moving right and not sprinting...
					if (StandingMovementStatus != EStandingMovementStatus::Esms_Sprinting)
					{
						//...then the player is jogging
						SetStandingMovementStatus(EStandingMovementStatus::Esms_Jogging);
					}
				}
				else if (StanceStatus == EStanceStatus::Ess_Crouching)
				{
					SetCrouchingMovementStatus(ECrouchingMovementStatus::Ecms_Walking);
				}
			}
		}
		//If player is not pressing S/D
		else
		{
			//...then player is not moving right
			bIsMovingRight = false;
		}
	}
}

void ASRCharacter::LookUp(float value)
{
	if(bGlobalMouseInput)
	{
		if(Pitch != 60 && Pitch != -60)
		{
			AddControllerPitchInput(value);
		}
		else if (Pitch != 60)
		{
			if (value < 0)
			{
				AddControllerPitchInput(value);
			}
		}
		else if(Pitch != -60)
		{
			if (value > 0)
			{
				AddControllerPitchInput(value);
			}
		}
	}
}

void ASRCharacter::Turn(float value)
{
	if (bGlobalMouseInput)
	{
		
		float valueMultiplier = 1.0f;

		if(StanceStatus !=EStanceStatus::Ess_Sliding)
		{
			valueMultiplier = 1.0f;
		}
		else
		{
			valueMultiplier = 0.25f;
		}
		/*
		if (Yaw <= 20 && Yaw >= -20)
		{
			bUseControllerRotationYaw = false;
				
		}
		else if (Yaw >= 60)
		{
			bUseControllerRotationYaw = true;
			if (value < 0)
			{

			}
		}
		else if (Yaw <= -60)
		{
			bUseControllerRotationYaw = true;
			if (value > 0)
			{

			}
		}*/
		AddControllerYawInput(value*valueMultiplier);
	}
}

void ASRCharacter::CheckCapsuleHeightRadius()
{
	bool bHeightDone = false;
	bool bRadiusDone = false;
	
	if(GetStanceStatus() == EStanceStatus::Ess_Standing || GetInAirStatus() != EInAirStatus::Eias_Nis)
	{
		GetMesh()->SetRelativeLocation(FVector(-5, 0, -88));
		if (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() < 87.0f || GetCapsuleComponent()->GetScaledCapsuleHalfHeight() > 89.0f)
		{
			GetCapsuleComponent()->SetCapsuleHalfHeight(FMath::Lerp(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), StandingCapsuleHeight, 0.2));
		}
		else
		{
			bHeightDone = true;
		}
		
		if (GetCapsuleComponent()->GetScaledCapsuleRadius() < 19.0f || GetCapsuleComponent()->GetScaledCapsuleRadius() > 21.0f)
		{
			GetCapsuleComponent()->SetCapsuleRadius(FMath::Lerp(GetCapsuleComponent()->GetScaledCapsuleRadius(), StandingCapsuleRadius, 0.2));
		}
		else
		{
			bRadiusDone = true;
		}

		if(bHeightDone && bRadiusDone)
		{
			bCheckCapsuleProperties = false;
		}
	}
	else if(GetStanceStatus() == EStanceStatus::Ess_Crouching)
	{
		GetMesh()->SetRelativeLocation(FVector(0, 0, -63));
		if (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() < 62 || GetCapsuleComponent()->GetScaledCapsuleHalfHeight() > 64)
		{
			GetCapsuleComponent()->SetCapsuleHalfHeight(FMath::Lerp(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), CrouchingCapsuleHeight, 0.2));
		}
		else
		{
			bHeightDone = true;
		}
		if (GetCapsuleComponent()->GetScaledCapsuleRadius() < 26 || GetCapsuleComponent()->GetScaledCapsuleRadius() > 28)
		{
			GetCapsuleComponent()->SetCapsuleRadius(FMath::Lerp(GetCapsuleComponent()->GetScaledCapsuleRadius(), CrouchingCapsuleRadius, 0.2));
		}
		else
		{
			bRadiusDone = true;
		}

		if (bHeightDone && bRadiusDone)
		{
			bCheckCapsuleProperties = false;
		}
}
	else if(GetStanceStatus() == EStanceStatus::Ess_Sliding)
	{
		GetMesh()->SetRelativeLocation(FVector(0, -25, -25));
		if (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() < 29 || GetCapsuleComponent()->GetScaledCapsuleHalfHeight() > 31)
		{
			GetCapsuleComponent()->SetCapsuleHalfHeight(FMath::Lerp(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), SlidingCapsuleHeight, 0.4));
		}
		else
		{
			bHeightDone = true;
		}
		if (GetCapsuleComponent()->GetScaledCapsuleRadius() < 29 || GetCapsuleComponent()->GetScaledCapsuleRadius() > 31)
		{
			GetCapsuleComponent()->SetCapsuleRadius(FMath::Lerp(GetCapsuleComponent()->GetScaledCapsuleRadius(), SlidingCapsuleRadius, 0.4));
		}
		else
		{
			bRadiusDone = true;
		}
		if (bHeightDone && bRadiusDone)
		{
			bCheckCapsuleProperties = false;
		}
	}

}

void ASRCharacter::CrouchSlideCheckPressed()
{
	if(StanceStatus != EStanceStatus::Ess_InAir)
	{
		if (!bJustPressedSprint)
		{
			BeginCrouch();
		}
		else
		{
			if (bIsMovingForward)
			{
				StartSlide();
				//SetStanceStatus(EStanceStatus::Ess_Crouching);
				//SetStandingMovementStatus(EStandingMovementStatus::Esms_Nis);
				//SetCrouchingMovementStatus(ECrouchingMovementStatus::Ecms_Idling);
			}
		}
	}
	else
	{
		if (bIsMovingForward)
		{
			SlideCheck = true;
			SlideRequest = true;
		}

	}
	if(bIsMovingForward)
	{
		SlideRequest = true;
	}

}

void ASRCharacter::CrouchSlideCheckReleased()
{
	if (StanceStatus != EStanceStatus::Ess_InAir)
	{
		if (StanceStatus != EStanceStatus::Ess_Sliding)
		{
			EndCrouch();
		}
		else
		{
			EndSlide();
		}
	}
	else
	{
		SetStandingMovementStatus(EStandingMovementStatus::Esms_Jogging);
		SetCrouchingMovementStatus(ECrouchingMovementStatus::Ecms_Nis);
	}
	
	SlideRequest = false;
	SlideCheck = false;
}

void ASRCharacter::AimPressed()
{
	if (bGunHolstered == false)
	{

		SetGunStatus(EGunStatus::Egs_ADSing);
		bChangeFOV = true;
		if(StandingMovementStatus == EStandingMovementStatus::Esms_Sprinting)
		{
			SetStandingMovementStatus(EStandingMovementStatus::Esms_Jogging);
		}
		if(StanceStatus == EStanceStatus::Ess_Sliding)
		{
			EndSlide();
		}
		
		UE_LOG(LogTemp, Warning, TEXT("AIMING"));
	}
}

void ASRCharacter::AimReleased()
{
	if (bGunHolstered == false)
	{

		SetGunStatus(EGunStatus::Egs_Nis);
		bChangeFOV = true;
		UE_LOG(LogTemp, Warning, TEXT("not AIMING"));
	}
}

void ASRCharacter::EquipPrimaryWeapon()
{
	bGunHolstered = !bGunHolstered;
}

void ASRCharacter::SetCameraFOV(float DeltaTime)
{
	float CurrentFov = CameraComp->FieldOfView;
	float Target;
	
	if(GunStatus == EGunStatus::Egs_ADSing)
	{
		Target = ADSCameraFOV;
	}
	else if(GunStatus == EGunStatus::Egs_Nis)
	{
		Target = WhipCameraFOV;
	}
	else if(GunStatus == EGunStatus::Egs_Reloading)
	{
		Target = WhipCameraFOV;
	}
	else
	{
		Target = WhipCameraFOV;
	}
	
	if(CurrentFov != Target)
	{
		CameraComp->SetFieldOfView(FMath::FInterpTo(CurrentFov, Target, DeltaTime,ZoomInterpSpeed));
	}
	else
	{
		bChangeFOV = false;
	}
}

void ASRCharacter::GlobalKeysInputDisable()
{
	bGlobalKeysInput = false;
}

void ASRCharacter::GlobalKeysInputEnable()
{
	bGlobalKeysInput = true;
}

void ASRCharacter::GlobalMouseInputDisable()
{
	bGlobalMouseInput = false;
}	

void ASRCharacter::GlobalMouseInputEnable()
{
	bGlobalMouseInput = true;
}

void ASRCharacter::CrouchToggle()
{
	//Toggle crouch boolean
	bToggleCrouch = !bToggleCrouch;
	
	if(bToggleCrouch)
	{
		BeginCrouch();
	}
	else
	{
		EndCrouch();
	}
}

void ASRCharacter::BeginCrouch()
{
	if(!bJustPressedSprint)
	{
		if(GetCharacterMovementSpeed() < 800)
		{
			SetStanceStatus(EStanceStatus::Ess_Crouching);
			SetStandingMovementStatus(EStandingMovementStatus::Esms_Nis);
			SetCrouchingMovementStatus(ECrouchingMovementStatus::Ecms_Idling);
			GetMesh()->SetRelativeLocation(FVector(0, 0, -63));
			bCheckCapsuleProperties = true;
			SetCharacterMovementSpeed(CrouchSpeed);
		}		
	}
	else
	{
		//StartSlide();
	}	
}

void ASRCharacter::EndCrouch()
{
	SetStanceStatus(EStanceStatus::Ess_Standing);
	SetStandingMovementStatus(EStandingMovementStatus::Esms_Idling);
	SetCrouchingMovementStatus(ECrouchingMovementStatus::Ecms_Nis);
	GetMesh()->SetRelativeLocation(FVector(0, 0, -88));
	bCheckCapsuleProperties = true;
}

bool ASRCharacter::GetGunHolstered()
{
	return bGunHolstered;
}


bool ASRCharacter::GetShouldHardLand()
{
	return bShouldHardLand;
	/*if(bShouldHardLand)
	{
		return true;
	}
	else
	{
		return false;
	}*/
}

void ASRCharacter::SetCharacterMovementSpeed(float MoveSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
}

float ASRCharacter::GetCharacterMovementSpeed()
{
	return GetCharacterMovement()->MaxWalkSpeed;
}

void ASRCharacter::StartSprint()
{
	// a delay when letting go of sprint to be able to get into sliding state
	if (GetWorld()->GetTimerManager().IsTimerActive(TimerEndSprint))
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerEndSprint);
	}
	if(GetStanceStatus()==EStanceStatus::Ess_Crouching)
	{
		SlideRequest = false;
		bCheckCapsuleProperties = true;
		EndCrouch();
	}
	if(GetStanceStatus() ==EStanceStatus::Ess_Sliding)
	{
		EndSlide();
	}

	if (StanceStatus == EStanceStatus::Ess_Standing)
	{
		bJustPressedSprint = true;
	}

	SetStanceStatus(EStanceStatus::Ess_Standing);
	SetStandingMovementStatus(EStandingMovementStatus::Esms_Sprinting);
	SetCrouchingMovementStatus(ECrouchingMovementStatus::Ecms_Nis);
	
}

void ASRCharacter::SprintReleased()
{
	GetWorld()->GetTimerManager().SetTimer(TimerEndSprint, this, &ASRCharacter::JustPressedSprint, EndSprintDelay, false);
	EndSprint();
}

void ASRCharacter::EndSprint()
{
	if(StanceStatus != EStanceStatus::Ess_Sliding)
	{
		if (StanceStatus != EStanceStatus::Ess_Crouching)
		{
			SetStandingMovementStatus(EStandingMovementStatus::Esms_Jogging);
		}
	}
}

void ASRCharacter::JustPressedSprint()
{
	bJustPressedSprint = false;
}

void ASRCharacter::StartJump()
{
	if(StanceStatus == EStanceStatus::Ess_Crouching)
	{
		UnCrouch();
		SetStanceStatus(EStanceStatus::Ess_Standing);
		SetStandingMovementStatus(EStandingMovementStatus::Esms_Idling);
		SetCrouchingMovementStatus(ECrouchingMovementStatus::Ecms_Nis);	
	}
	else if(StanceStatus == EStanceStatus::Ess_Sliding)
	{
		EndSlide();
		SetStanceStatus(EStanceStatus::Ess_InAir);
		SetInAirStatus(EInAirStatus::Eias_Jumping);
		Jump();

	}
	else
	{
		SetStanceStatus(EStanceStatus::Ess_InAir);
		SetInAirStatus(EInAirStatus::Eias_Jumping);
		Jump();
	}
	

}

void ASRCharacter::StartSlide()
{
	if(bIsMovingForward)
	{
		SetStanceStatus(EStanceStatus::Ess_Sliding);
		bCheckCapsuleProperties = true;
		GetMesh()->SetRelativeLocation(FVector(0, -25, -25));

		if (GetWorld()->GetTimerManager().IsTimerActive(TimerEndSprint))
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerEndSprint);
		}
		bJustPressedSprint = false;
		
	}
}

void ASRCharacter::EndSlide()
{
	SetStanceStatus(EStanceStatus::Ess_Standing);
	UE_LOG(LogTemp, Warning, TEXT("END SLIDE"));
	if(!SlideRequest)
	{

		GetMesh()->SetRelativeLocation(FVector(-5, 0, -88));
		if ((bIsMovingForward || bIsMovingRight))
		{
			if (StandingMovementStatus == EStandingMovementStatus::Esms_Jogging || StandingMovementStatus == EStandingMovementStatus::Esms_Sprinting)
			{
				SetStandingMovementStatus(EStandingMovementStatus::Esms_Jogging);
			}
		}
		else
		{
			SetStandingMovementStatus(EStandingMovementStatus::Esms_Idling);
		}
	}
	else if(SlideRequest && GetInAirStatus() == EInAirStatus::Eias_Nis)
	{
		BeginCrouch();
		GetMesh()->SetRelativeLocation(FVector(0, 0, -63));
		if ((bIsMovingForward || bIsMovingRight))
		{
			SetCrouchingMovementStatus(ECrouchingMovementStatus::Ecms_Walking);
		}
		else
		{
			SetCrouchingMovementStatus(ECrouchingMovementStatus::Ecms_Idling);
		}
	}

	bCheckCapsuleProperties = true;
	SlideRequest = false;
}

void ASRCharacter::SlideSlopeDetection()
{
	FHitResult SlopeAngleTraceHit;

	FCollisionQueryParams QueryParams;
	FVector SlopeAngleTraceEnd = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z - SlideTraceLength);
	FVector SlopeAngleTraceStart = GetActorLocation();
	
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = false;
	
	if(GetWorld()->LineTraceSingleByChannel(SlopeAngleTraceHit, SlopeAngleTraceStart, SlopeAngleTraceEnd, ECC_Visibility, QueryParams))
	{
		//Hit
		DrawDebugLine(GetWorld(), SlopeAngleTraceStart, SlopeAngleTraceEnd, FColor::Green,false,3,0,1);
		//UE_LOG(LogTemp, Warning, TEXT("%f"), SlopeAngleTraceHit.ImpactNormal.Z);
		if(SlopeAngleTraceHit.ImpactNormal.Z >= 1.0f)
		{
			SetSlideStatus(ESlideStatus::Ess_FlatSlope);
		}
		else if(SlopeAngleTraceHit.ImpactNormal.Z < 1.0f && SlopeAngleTraceHit.ImpactNormal.Z >= 0.9f)
		{
			SetSlideStatus(ESlideStatus::Ess_SlantedSlope);
			//SlideRequest = true;
		}	
		else
		{
			SetSlideStatus(ESlideStatus::Ess_SteepSlope);
			//SlideRequest = true;
		}
		
		FHitResult UpDownHillTraceHit;
		FVector ForwardVector = CameraComp->GetUpVector();
		FVector UpDownHillStartTrace = FVector(SlopeAngleTraceHit.Location.X, SlopeAngleTraceHit.Location.Y, SlopeAngleTraceHit.Location.Z);
		FVector UpDownHillTrueStartTrace = (ForwardVector * 100.0f) + UpDownHillStartTrace; //(ForwardVector * 600.0f) +
		FVector UpDownHillEndTrace = FVector(UpDownHillTrueStartTrace.X, UpDownHillTrueStartTrace.Y, UpDownHillTrueStartTrace.Z - 950.0f); //- 700.0f);

		DrawDebugLine(GetWorld(), UpDownHillTrueStartTrace, UpDownHillEndTrace, FColor::Red, false, 8, 0, 9);

		if(GetWorld()->LineTraceSingleByChannel(UpDownHillTraceHit, UpDownHillTrueStartTrace, UpDownHillEndTrace, ECC_Visibility, QueryParams))
		{
			float Startpoint = SlopeAngleTraceHit.ImpactPoint.Z;
			float EndPoint = UpDownHillTraceHit.ImpactPoint.Z;
			float PointDifference = Startpoint - EndPoint;
			if(PointDifference < -0.15f)
			{
				EndSlide();
			}
			UE_LOG(LogTemp, Warning, TEXT("%f"), PointDifference);
		}
		else
		{
		}
	}
	else
	{
		SetSlideStatus(ESlideStatus::Ess_NoSlope);
	}
}

void ASRCharacter::SlideSpeedCalculation()
{
	if (SlideStatus == ESlideStatus::Ess_FlatSlope)
	{
		if(GetCharacterMovementSpeed() > 850)
		{
			if(GetCharacterMovementSpeed() > 1650)
			{
				SetCharacterMovementSpeed(GetCharacterMovementSpeed()*FastSpeedLoss);
			}
			else
			{
				SetCharacterMovementSpeed(GetCharacterMovementSpeed()*SlowSpeedLoss);
			}

		}
		else
		{
			EndSlide();
		}	
	}
	else if (SlideStatus == ESlideStatus::Ess_SlantedSlope)
	{
		SetCharacterMovementSpeed(GetCharacterMovementSpeed()*SlowSpeedGain);
	}
	else if(SlideStatus == ESlideStatus::Ess_SteepSlope)
	{
		SetCharacterMovementSpeed(GetCharacterMovementSpeed()*FastSpeedGain);
	}
	else
	{
		SetSlideStatus(ESlideStatus::Ess_FlatSlope);
	}
}

void ASRCharacter::FreeLookOn()
{
	bUseControllerRotationYaw = false;
}

void ASRCharacter::FreeLookOff()
{
	bUseControllerRotationYaw = true;
}


// Stance, Crouching, Standing, InAir Status
#pragma region Statuses Getters & Setters

void ASRCharacter::SetStanceStatus(EStanceStatus Status)
{
	//Set Stance movement status to the input status
	StanceStatus = Status;

	if (StanceStatus == EStanceStatus::Ess_Standing)
	{
		GetMesh()->SetRelativeLocation(FVector(-5, 0, -88));
	}
	else if (StanceStatus == EStanceStatus::Ess_Crouching)
	{
		GetMesh()->SetRelativeLocation(FVector(0, 0, -63));
	}
	else if(StanceStatus == EStanceStatus::Ess_Sliding)
	{
		GetMesh()->SetRelativeLocation(FVector(0, -25, -25));
		
		if(GetCharacterMovementSpeed()<SlideSpeed)
		{
			SetCharacterMovementSpeed(SlideSpeed);
		}
	}
}

void ASRCharacter::SetSlideStatus(ESlideStatus Status)
{
	SlideStatus = Status;
}

void ASRCharacter::SetGunStatus(EGunStatus Status)
{
	GunStatus = Status;
}

EStanceStatus ASRCharacter::GetStanceStatus()
{
	return StanceStatus;
}

void ASRCharacter::SetStandingMovementStatus(EStandingMovementStatus Status)
{
	//Set Standing movement status to the input status
	StandingMovementStatus = Status;

	if (StandingMovementStatus == EStandingMovementStatus::Esms_Jogging)
	{
		
	}
	else if (StandingMovementStatus == EStandingMovementStatus::Esms_Sprinting)
	{
		if (bIsMovingRight)
		{
			//SetCharacterMovementSpeed(DiagonalSprintSpeed);
			//GetCharacterMovement()->MaxWalkSpeed = DiagonalSprintSpeed;
		}
		else
		{
			//SetCharacterMovementSpeed(SprintSpeed);
			//GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		}

	}
	else if (StandingMovementStatus == EStandingMovementStatus::Esms_Nis)
	{

	}
}

EStandingMovementStatus ASRCharacter::GetStandingMovementStatus()
{
	return StandingMovementStatus;
}

void ASRCharacter::SetCrouchingMovementStatus(ECrouchingMovementStatus Status)
{
	//Set Crouching movement status to the input status
	CrouchingMovementStatus = Status;
	//SetCharacterMovementSpeed(

	if (CrouchingMovementStatus == ECrouchingMovementStatus::Ecms_Idling)
	{

	}
	else if (CrouchingMovementStatus == ECrouchingMovementStatus::Ecms_Walking)
	{
		
	}
}

ECrouchingMovementStatus ASRCharacter::GetCrouchingMovementStatus()
{
	return CrouchingMovementStatus;
}

void ASRCharacter::SetInAirStatus(EInAirStatus Status)
{
	InAirStatus = Status;
}

EInAirStatus ASRCharacter::GetInAirStatus()
{
	return InAirStatus;
}

EGunStatus ASRCharacter::GetGunStatus()
{
	return GunStatus; 
}



#pragma endregion 

