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
	
	if(StanceStatus == EStanceStatus::Ess_Sliding)
	{
		SlideSlopeDetection();
		SlideSpeedCalculation();
	}
	else if(SlideCheck && StanceStatus != EStanceStatus::Ess_Sliding)
	{
		SlideSlopeDetection();
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

}

void ASRCharacter::Landed(const FHitResult & Hit)
{
	Super::Landed(Hit);
	// Land and the character is Flailing
	if(InAirStatus == EInAirStatus::Eias_Flailing)
	{
		if(SlideRequest && (SlideStatus == ESlideStatus::Eias_SlantedSlope || SlideStatus == ESlideStatus::Eias_SteepSlope))
		{
			SetStandingMovementStatus(EStandingMovementStatus::Esms_Nis);
			SetCrouchingMovementStatus(ECrouchingMovementStatus::Ecms_Nis);
			StartSlide();	
		}
		else
		{
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

	
	if (!SlideRequest)
	{
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
		SetStandingMovementStatus(EStandingMovementStatus::Esms_Nis);
		SetCrouchingMovementStatus(ECrouchingMovementStatus::Ecms_Nis);
		StartSlide();
	}
	
	SetInAirStatus(EInAirStatus::Eias_Nis);
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
						SetCharacterMovementSpeed(JogSpeed);

						//GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
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
		
		//If player is pressing S/D...
		if (value != 0)
		{
			//...then player is moving right
			bIsMovingRight = true;

			if(!bIsMovingForward)
			{
				SetCharacterMovementSpeed(JogSpeed);
			}
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

void ASRCharacter::CrouchSlideCheckPressed()
{
	if(StanceStatus != EStanceStatus::Ess_InAir)
	{
		if (StandingMovementStatus != EStandingMovementStatus::Esms_Sprinting)
		{
			BeginCrouch();
		}
		else
		{
			StartSlide();
		}
	}
	else
	{
		SlideCheck = true;
		
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
	
	SlideRequest = false;
	SlideCheck = false;
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
	if(StandingMovementStatus != EStandingMovementStatus::Esms_Sprinting)
	{
		Crouch();
		SetStanceStatus(EStanceStatus::Ess_Crouching);
		SetStandingMovementStatus(EStandingMovementStatus::Esms_Nis);
		SetCrouchingMovementStatus(ECrouchingMovementStatus::Ecms_Idling);
	}
	else
	{
		StartSlide();
	}
	
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
	if(StanceStatus == EStanceStatus::Ess_Crouching)
	{
		EndCrouch();
	}
	SetStanceStatus(EStanceStatus::Ess_Standing);
	SetStandingMovementStatus(EStandingMovementStatus::Esms_Sprinting);
	SetCrouchingMovementStatus(ECrouchingMovementStatus::Ecms_Nis);
}

void ASRCharacter::SprintReleased()
{
	GetWorld()->GetTimerManager().SetTimer(TimerEndSprint, this, &ASRCharacter::EndSprint, EndSprintDelay, false);
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
	SetStanceStatus(EStanceStatus::Ess_Sliding);
}

void ASRCharacter::EndSlide()
{
	SetStanceStatus(EStanceStatus::Ess_Standing);
	SlideRequest = false;
	if(GetWorld()->GetTimerManager().IsTimerActive(TimerSlideDuration))
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerSlideDuration);
	}
	
	if (bIsMovingForward || bIsMovingRight)
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
		
		if(SlopeAngleTraceHit.ImpactNormal.Z >= 1.0f)
		{
			SetSlideStatus(ESlideStatus::Eias_FlatSlope);
		}
		else if(SlopeAngleTraceHit.ImpactNormal.Z < 1.0f && SlopeAngleTraceHit.ImpactNormal.Z >= 0.9f)
		{
			SetSlideStatus(ESlideStatus::Eias_SlantedSlope);
			SlideRequest = true;
		}
		else
		{
			SetSlideStatus(ESlideStatus::Eias_SteepSlope);
			SlideRequest = true;
		}

		FHitResult UpDownHillTraceHit;
		FVector ForwardVector = CameraComp->GetForwardVector();
		FVector UpDownHillStartTrace = FVector(SlopeAngleTraceHit.Location.X, SlopeAngleTraceHit.Location.Y, SlopeAngleTraceHit.Location.Z + 90);
		FVector UpDownHillEndTrace = (ForwardVector * 300.0f) + UpDownHillStartTrace;

		DrawDebugLine(GetWorld(), UpDownHillStartTrace, UpDownHillEndTrace, FColor::Red, false, 3, 0, 2);
		
		if(GetWorld()->LineTraceSingleByChannel(UpDownHillTraceHit, UpDownHillStartTrace, UpDownHillEndTrace, ECC_Visibility, QueryParams))
		{
			UE_LOG(LogTemp, Warning, TEXT("UPHILLL!"));
			SetStanceStatus(EStanceStatus::Ess_Standing);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("DOWNHILLL!"))
		}
	}
	else
	{
		SetSlideStatus(ESlideStatus::Eias_NoSlope);
	}
}

void ASRCharacter::SlideSpeedCalculation()
{
	if (SlideStatus == ESlideStatus::Eias_FlatSlope)
	{
		if(GetCharacterMovementSpeed() > 850)
		{
			if(GetCharacterMovementSpeed() > 1650)
			{
				SetCharacterMovementSpeed(GetCharacterMovementSpeed()*0.8f);
			}
			else
			{
				SetCharacterMovementSpeed(GetCharacterMovementSpeed()*0.93f);
			}

		}
		else
		{
			SetStanceStatus(EStanceStatus::Ess_Standing);		
		}	
	}
	else if (SlideStatus == ESlideStatus::Eias_SlantedSlope)
	{
		SetCharacterMovementSpeed(GetCharacterMovementSpeed()*1.05f);
	}
	else if(SlideStatus == ESlideStatus::Eias_SteepSlope)
	{
		SetCharacterMovementSpeed(GetCharacterMovementSpeed()*1.1f);
	}
	else
	{
		SetSlideStatus(ESlideStatus::Eias_FlatSlope);
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
	else if(StanceStatus == EStanceStatus::Ess_Sliding)
	{
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
			SetCharacterMovementSpeed(DiagonalSprintSpeed);
			//GetCharacterMovement()->MaxWalkSpeed = DiagonalSprintSpeed;
		}
		else
		{
			SetCharacterMovementSpeed(SprintSpeed);
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

/*ESlideStatus ASRCharacter::GetSlideStatus()
{
	//return SlideStatus();
}*/

#pragma endregion 

