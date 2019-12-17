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
	GetCharacterMovement()->CrouchedHalfHeight = 63.0f;
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
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASRCharacter::CrouchToggle);
	PlayerInputComponent->BindAction("FreeLook", IE_Pressed, this, &ASRCharacter::FreeLookOn);
	PlayerInputComponent->BindAction("FreeLook", IE_Released, this, &ASRCharacter::FreeLookOff);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASRCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASRCharacter::EndSprint);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASRCharacter::StartJump);

}

void ASRCharacter::Landed(const FHitResult & Hit)
{
	Super::Landed(Hit);
	// Land and the character is Flailing
	if(InAirStatus == EInAirStatus::Eias_Flailing)
	{
		GlobalKeysInputDisable();
		GlobalMouseInputDisable();
		if(FallHeight > 1300.0f)
		{
			LandDelay = HardLandDelay;
			bShouldHardLand = true;
		}
		else
		{
			LandDelay = SoftLandDelay;
			bShouldHardLand = false;
		}
		GetWorld()->GetTimerManager().SetTimer(TimerGlobalKeysInput, this, &ASRCharacter::GlobalKeysInputEnable,LandDelay,false);
		GetWorld()->GetTimerManager().SetTimer(TimerGlobalMouseInput, this, &ASRCharacter::GlobalMouseInputEnable, LandDelay, false);
	}

	SetInAirStatus(EInAirStatus::Eias_Nis);
	
	if(StandingMovementStatus == EStandingMovementStatus::Esms_Jogging || StandingMovementStatus == EStandingMovementStatus::Esms_Idling 
		|| StandingMovementStatus == EStandingMovementStatus::Esms_Sprinting)
	{
		SetStanceStatus(EStanceStatus::Ess_Standing);
	}
	else if(CrouchingMovementStatus == ECrouchingMovementStatus::Ecms_Idling || CrouchingMovementStatus == ECrouchingMovementStatus::Ecms_Walking)
	{
		SetStanceStatus(EStanceStatus::Ess_Crouching);
	}

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
					//...and player is moving forward and sprinting...
					if (StandingMovementStatus == EStandingMovementStatus::Esms_Sprinting)
					{
						if (bIsMovingRight)
						{
							SprintSpeed = DiagonalSprintSpeed;
							SetStandingMovementStatus(EStandingMovementStatus::Esms_Sprinting);
						}
						else
						{
							SprintSpeed = DefaultSprintSpeed;
							SetStandingMovementStatus(EStandingMovementStatus::Esms_Sprinting);
						}
					}
					//...and player is moving forward and not sprinting...
					else
					{
						//...then player is jogging
						SetStandingMovementStatus(EStandingMovementStatus::Esms_Jogging);
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
		//...whilst crouchign
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
		
		//If player is pressing S/D...
		if (value != 0)
		{
			//...then player is moving right
			bIsMovingRight = true;

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
		AddControllerPitchInput(value);
	}
}

void ASRCharacter::Turn(float value)
{
	if (bGlobalMouseInput)
	{
		AddControllerYawInput(value);
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
	Crouch();
	SetStanceStatus(EStanceStatus::Ess_Crouching);
	SetStandingMovementStatus(EStandingMovementStatus::Esms_Nis);
	SetCrouchingMovementStatus(ECrouchingMovementStatus::Ecms_Idling);
}

void ASRCharacter::EndCrouch()
{
	UnCrouch();
	SetStanceStatus(EStanceStatus::Ess_Standing);
	SetStandingMovementStatus(EStandingMovementStatus::Esms_Idling);
	SetCrouchingMovementStatus(ECrouchingMovementStatus::Ecms_Nis);
	
}

bool ASRCharacter::GetIsArmed()
{
	return bIsArmed;
}

bool ASRCharacter::GetShouldHardLand()
{
	if(bShouldHardLand)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ASRCharacter::StartSprint()
{
	if(StanceStatus == EStanceStatus::Ess_Crouching)
	{
		EndCrouch();
	}
	SetStanceStatus(EStanceStatus::Ess_Standing);
	SetStandingMovementStatus(EStandingMovementStatus::Esms_Sprinting);
	SetCrouchingMovementStatus(ECrouchingMovementStatus::Ecms_Nis);
}

void ASRCharacter::EndSprint()
{
	if(StanceStatus != EStanceStatus::Ess_Crouching)
	{
		SetStandingMovementStatus(EStandingMovementStatus::Esms_Jogging);
	}
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
	else
	{
		SetStanceStatus(EStanceStatus::Ess_InAir);
		SetInAirStatus(EInAirStatus::Eias_Jumping);
		//SetStandingMovementStatus(EStandingMovementStatus::Esms_Nis);
		//SetCrouchingMovementStatus(ECrouchingMovementStatus::Ecms_Nis);
		Jump();
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

	}
	else if (StanceStatus == EStanceStatus::Ess_Crouching)
	{

	}
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
		GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
	}
	else if (StandingMovementStatus == EStandingMovementStatus::Esms_Sprinting)
	{
		if (bIsMovingRight)
		{
			GetCharacterMovement()->MaxWalkSpeed = DiagonalSprintSpeed;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
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

#pragma endregion 

