// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicles/SpaceShip/SRSpaceShip.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/PrimitiveComponent.h"

// Sets default values
ASRSpaceShip::ASRSpaceShip()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Ship Mesh
	ShipMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShipMesh"));
	RootComponent = ShipMesh;
		//Physics
	ShipMesh->SetSimulatePhysics(true);
	ShipMesh->bReplicatePhysicsToAutonomousProxy = false;
		// Collision
	ShipMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ShipMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	ShipMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	ShipMesh->SetGenerateOverlapEvents(true);
	ShipMesh->SetNotifyRigidBodyCollision(true);
	// Spring Arm
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bEnableCameraLag = true;
	SpringArmComp->bEnableCameraRotationLag = true;
	SpringArmComp->CameraRotationLagSpeed = 100.0f;
	SpringArmComp->CameraLagMaxDistance = 5000.0f;
	SpringArmComp->CameraLagSpeed = 100.0f;

	// Camera
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;
}

// Called when the game starts or when spawned
void ASRSpaceShip::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASRSpaceShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Torque = (-ShipMesh->GetPhysicsAngularVelocityInDegrees(NAME_None)) / 1.0f;
	ShipMesh->AddTorqueInDegrees(Torque, NAME_None, true);
}

void ASRSpaceShip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	//Axis Events

	PlayerInputComponent->BindAxis("ShipLookUp", this, &ASRSpaceShip::LookUp);
	PlayerInputComponent->BindAxis("ShipLookRight", this, &ASRSpaceShip::LookRight);
	PlayerInputComponent->BindAxis("ShipMoveForward", this, &ASRSpaceShip::MoveForward);
	PlayerInputComponent->BindAxis("ShipMoveRight", this, &ASRSpaceShip::MoveRight);
	PlayerInputComponent->BindAxis("ShipRollRight", this, &ASRSpaceShip::RollRight);
	//PlayerInputComponent->BindAxis("ShipTurn", this, &ASRSpaceShip::Turn);
}

#pragma region Input 
void ASRSpaceShip::LookUp(float value)
{
	FVector Torque = FMath::Lerp(ZeroVector, 
		GetActorRightVector() * (value * RotationSpeed),
		PitchAcceleration);
	ShipMesh->AddTorqueInDegrees(Torque, NAME_None, true);

	//UE_LOG(LogTemp, Warning, TEXT("Looking Up!"));
}

void ASRSpaceShip::LookRight(float value)
{
	FVector Torque = FMath::Lerp(ZeroVector, 
		GetActorUpVector() * (value * RotationSpeed),
		YawAcceleration);
	
	ShipMesh->AddTorqueInDegrees(Torque, NAME_None, true);
	
	//UE_LOG(LogTemp, Warning, TEXT("Looking Right!"));
}

void ASRSpaceShip::MoveForward(float value)
{
	CurrentForwardSpeed = FMath::Lerp(CurrentForwardSpeed, TargetForwardSpeed, ForwardLinearPhysicsAlpha);
	
	FVector CurrentLinearVelocity = ShipMesh->GetPhysicsLinearVelocity(NAME_None);

	FVector TargetLinearVelocity = GetActorForwardVector() * (value*CurrentForwardSpeed);

	
	//FVector NewVelocity = ZeroVector;
	if(value != 0)
	{
		if(value > 0)
		{
			TargetForwardSpeed = ForwardSpeed;
			ForwardLinearPhysicsAlpha = ForwardAcceleration;
		}
		else
		{
			TargetForwardSpeed = BackwardSpeed;
			ForwardLinearPhysicsAlpha = BackwardAcceleration;
		}
	}
	else
	{
		TargetForwardSpeed = 0;
		ForwardLinearPhysicsAlpha = ForwardDeceleration;
	}
	
	NewVelocity = FMath::Lerp(CurrentLinearVelocity, TargetLinearVelocity, ForwardLinearPhysicsAlpha);
	
	ShipMesh->SetPhysicsLinearVelocity(NewVelocity, false, NAME_None);
}

void ASRSpaceShip::MoveRight(float value)
{
	CurrentRightSpeed = FMath::Lerp(CurrentRightSpeed, TargetRightSpeed, RightLinearPhysicsAlpha);

	FVector CurrentLinearVelocity = ShipMesh->GetPhysicsLinearVelocity(NAME_None);

	FVector TargetLinearVelocity = GetActorRightVector() * (value*CurrentRightSpeed);

	if (value != 0)
	{
		TargetRightSpeed = RightSpeed;
		RightLinearPhysicsAlpha = RightAcceleration;
	}
	else
	{
		TargetRightSpeed = 0;
		RightLinearPhysicsAlpha = RightDeceleration;
	}

	NewVelocity = FMath::Lerp(CurrentLinearVelocity, TargetLinearVelocity, RightLinearPhysicsAlpha);

	ShipMesh->SetPhysicsLinearVelocity(NewVelocity, false, NAME_None);
}

void ASRSpaceShip::RollRight(float value)
{
	float RollVelocityChange = 0.0f;
	if (value != 0)
	{
		RollVelocityChange = RollAcceleration;
	}
	else
	{
		RollVelocityChange = RollDeceleration;
	}
	FVector Torque = FMath::Lerp(ZeroVector,
		GetActorForwardVector() * (value * RollSpeed),
		RollVelocityChange);

	ShipMesh->AddTorqueInDegrees(Torque, NAME_None, true);

	//UE_LOG(LogTemp, Warning, TEXT("Looking Right!"));
}
#pragma endregion 

