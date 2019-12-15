// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Character/SRCharacter.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

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

	//Movement Component
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	// Input Properties
	bIsMovingForward = false;
	bIsMovingRight = false;
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

}

void ASRCharacter::MoveForward(float value)
{
	AddMovementInput(GetActorForwardVector() * value);

	//If player is pressing W/S...
	if(value != 0)
	{
		//...then player is moving forward
		bIsMovingForward = true;
		//...if player moving forward and not sprinting...
		if(MovementStatus != EMovementStatus::EMS_Sprinting)
		{
			//...then player is jogging
			SetMovementStatus(EMovementStatus::EMS_Jogging);
		}
	}
	//If Player is not pressing W/S...
	else
	{
		//...then player is not moving forward
		bIsMovingForward = false;
	}
	//If Player is not moving forward and not moving right...
	if(!bIsMovingForward && !bIsMovingRight)
	{
		//...then player is idling
		SetMovementStatus(EMovementStatus::EMS_Idling);
	}

}

void ASRCharacter::MoveRight(float value)
{
	AddMovementInput(GetActorRightVector()* value);

	//If player is pressing S/D...
	if (value != 0)
	{
		//...then player is moving right
		bIsMovingRight = true;
		//...if player moving right and not sprinting...
		if (MovementStatus != EMovementStatus::EMS_Sprinting)
		{
			//...then the player is jogging
			SetMovementStatus(EMovementStatus::EMS_Jogging);
		}
	}
	//If player is not pressing S/D
	else
	{
		//...then player is not moving right
		bIsMovingRight = false;
	}
}

void ASRCharacter::LookUp(float value)
{
	AddControllerPitchInput(value);
}

void ASRCharacter::Turn(float value)
{
	AddControllerYawInput(value);
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
}

void ASRCharacter::EndCrouch()
{
	UnCrouch();
}

void ASRCharacter::SetMovementStatus(EMovementStatus Status)
{
	//Set movement status to the input status
	MovementStatus = Status;

	if (MovementStatus == EMovementStatus::EMS_Jogging)
	{
		GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
	}
	else if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void ASRCharacter::StartSprint()
{
	SetMovementStatus(EMovementStatus::EMS_Sprinting);
}

void ASRCharacter::EndSprint()
{
	SetMovementStatus(EMovementStatus::EMS_Jogging);
}

void ASRCharacter::FreeLookOn()
{
	bUseControllerRotationYaw = false;
}
void ASRCharacter::FreeLookOff()
{
	bUseControllerRotationYaw = true;
}


