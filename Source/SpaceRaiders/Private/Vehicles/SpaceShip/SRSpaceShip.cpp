// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicles/SpaceShip/SRSpaceShip.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SphereComponent.h"
#include "Public/Character/SRCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

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
	SpringArmComp->CameraRotationLagSpeed = CameraRotationLagSpeed;
	SpringArmComp->CameraLagMaxDistance = CameraLagMaxDistance;
	SpringArmComp->CameraLagSpeed = CameraLagSpeed;

	// Camera
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	// Interactive Collision
	InteractCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PickUpCollision"));
	InteractCollision->SetupAttachment(RootComponent);
	InteractCollision->SetSphereRadius(700.0f);

	// Spawn Zone
	ExitShipZone = CreateDefaultSubobject<USceneComponent>(TEXT("ExitShipZone"));
	ExitShipZone->SetupAttachment(RootComponent);

	// Audio Component
	DirectionalMovementAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("DirectionalMovementAudioComp"));
}

// Called when the game starts or when spawned
void ASRSpaceShip::BeginPlay()
{
	Super::BeginPlay();

	// Pick Up Coll
	InteractCollision->OnComponentBeginOverlap.AddDynamic(this, &ASRSpaceShip::OnOverlapBegin);
	InteractCollision->OnComponentEndOverlap.AddDynamic(this, &ASRSpaceShip::OnOverlapEnd);
}

// Called every frame
void ASRSpaceShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//
	FVector Torque = (-ShipMesh->GetPhysicsAngularVelocityInDegrees(NAME_None)) / 1.0f;
	ShipMesh->AddTorqueInDegrees(Torque, NAME_None, true);
	
	if (!bShipOff)
	{

	}
	else
	{
		if(PilotClass)
		{
			MoveUp(-1.0f);
			BoostForwardReleased();
			
			FVector CurrentLinearVelocity = ShipMesh->GetPhysicsLinearVelocity(NAME_None);
			FVector TargetLinearVelocity = FVector(0, 0, 0);
			NewVelocity = FMath::Lerp(CurrentLinearVelocity, TargetLinearVelocity, 0.02);
			ShipMesh->SetPhysicsLinearVelocity(NewVelocity, false, NAME_None);
		}
	}
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
	PlayerInputComponent->BindAxis("ShipMoveUp", this, &ASRSpaceShip::MoveUp);
	
	// Action Mappings
	PlayerInputComponent->BindAction("ShipBoost", IE_Pressed, this, &ASRSpaceShip::BoostForwardPressed);
	PlayerInputComponent->BindAction("ShipBoost", IE_Released, this, &ASRSpaceShip::BoostForwardReleased);
	PlayerInputComponent->BindAction("ShipFreeLook", IE_Pressed, this, &ASRSpaceShip::FreeLookOn);
	PlayerInputComponent->BindAction("ShipFreeLook", IE_Released, this, &ASRSpaceShip::FreeLookOff);
	PlayerInputComponent->BindAction("ShipInteract", IE_Pressed, this, &ASRSpaceShip::ExitShip);
}


void ASRSpaceShip::ExitShip()
{
	{
		if (PilotClass && bCanExitShip)
		{
			UWorld* world = GetWorld();
			FActorSpawnParameters spawnParams;
			FRotator rotator;
			FVector spawnLocation;

			if (ExitShipZone)
			{
				spawnLocation = ExitShipZone->GetComponentLocation();
			}
			else
			{
				spawnLocation = GetActorLocation();
			}

			ASRCharacter* player = world->SpawnActor<ASRCharacter>(PilotClass, spawnLocation, rotator, spawnParams);
			
			player->GetWeapons(PrimaryWeapon, SecondaryWeapon);
			PrimaryWeapon = nullptr;
			SecondaryWeapon = nullptr;
			bShipOff = true;
		}
		
	}
}

void ASRSpaceShip::StoreWeapons(ASRGun * Primary, ASRGun * Secondary)
{
	if(Primary)
	{
		PrimaryWeapon = Primary;
	}
	if(Secondary)
	{
		SecondaryWeapon = Secondary;
	}
}



void ASRSpaceShip::OnOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ASRCharacter* myCharacter = Cast<ASRCharacter>(OtherActor);
	myCharacter->SetShip(this);
	PilotCloseby = myCharacter;
	
}

void ASRSpaceShip::OnOverlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	ASRCharacter* myCharacter = Cast<ASRCharacter>(OtherActor);
	myCharacter->SetShip(nullptr);
	PilotCloseby = nullptr;
}

#pragma region Input 
void ASRSpaceShip::LookUp(float value)
{
	if(!bShipOff)
	{
		FVector Torque = FMath::Lerp(ZeroVector,
			GetActorRightVector() * (value * RotationSpeed),
			PitchAcceleration);
		ShipMesh->AddTorqueInDegrees(Torque, NAME_None, true);

		//UE_LOG(LogTemp, Warning, TEXT("Looking Up!"));
	}

}

void ASRSpaceShip::LookRight(float value)
{
	if (!bShipOff)
	{

		FVector Torque = FMath::Lerp(ZeroVector,
			GetActorUpVector() * (value * RotationSpeed),
			YawAcceleration);

		ShipMesh->AddTorqueInDegrees(Torque, NAME_None, true);

		if (value > 0)
		{
			RollRight(-AutoRollAmount);
		}
		else if (value < 0)
		{
			RollRight(AutoRollAmount);
		}
	}
}

void ASRSpaceShip::MoveForward(float value)
{
	if (!bShipOff)
	{
		if (bIsBoosting)
		{
			CurrentForwardSpeed = FMath::Lerp(CurrentForwardSpeed, TargetForwardSpeed*BoostMultiplier*IndoorSpeedChange, ForwardLinearPhysicsAlpha*BoostMultiplier);

			FVector CurrentLinearVelocity = ShipMesh->GetPhysicsLinearVelocity(NAME_None);

			FVector TargetLinearVelocity = GetActorForwardVector() * FMath::Clamp(value*CurrentForwardSpeed, -BackwardSpeed, ForwardSpeed*BoostMultiplier*IndoorSpeedChange);

			// Moving forwards or backwards
			if (value != 0)
			{
				bIsMovingForward = true;
				//moving forwards
				if (value > 0)
				{
					// not moving right
					if (!bIsMovingRight)
					{
						TargetForwardSpeed = ForwardSpeed * BoostMultiplier*IndoorSpeedChange;
					}
					// is moving right
					else
					{
						TargetForwardSpeed = (ForwardSpeed* BoostMultiplier*IndoorSpeedChange) * 1.5f;
					}

					ForwardLinearPhysicsAlpha = ForwardAcceleration * BoostMultiplier*IndoorSpeedChange;
				}
				//moving backwards
				else
				{
					bIsBoosting = false;
					//not moving right
					if (!bIsMovingRight)
					{
						TargetForwardSpeed = BackwardSpeed * IndoorSpeedChange;
					}
					// is moving right
					else
					{
						TargetForwardSpeed = ForwardSpeed * 1.5f*IndoorSpeedChange;
					}

					ForwardLinearPhysicsAlpha = BackwardAcceleration * IndoorSpeedChange;
				}
			}
			// not moving forwards or backwards
			else
			{
				TargetForwardSpeed = 0;
				ForwardLinearPhysicsAlpha = ForwardDeceleration;
				bIsMovingForward = false;
			}

			NewVelocity = FMath::Lerp(CurrentLinearVelocity, TargetLinearVelocity, ForwardLinearPhysicsAlpha);
			ShipMesh->SetPhysicsLinearVelocity(NewVelocity, false, NAME_None);
		}
		else
		{
			CurrentForwardSpeed = FMath::Lerp(CurrentForwardSpeed, TargetForwardSpeed*IndoorSpeedChange, ForwardLinearPhysicsAlpha);

			FVector CurrentLinearVelocity = ShipMesh->GetPhysicsLinearVelocity(NAME_None);

			FVector TargetLinearVelocity = GetActorForwardVector() * FMath::Clamp(value*CurrentForwardSpeed, -BackwardSpeed, ForwardSpeed*IndoorSpeedChange);

			// Moving forwards or backwards
			if (value != 0)
			{
				bIsMovingForward = true;
				//moving forwards
				if (value > 0)
				{
					// not moving right
					if (!bIsMovingRight)
					{
						TargetForwardSpeed = ForwardSpeed * IndoorSpeedChange;
					}
					// is moving right
					else
					{
						TargetForwardSpeed = ForwardSpeed * 1.5f*IndoorSpeedChange;
					}

					ForwardLinearPhysicsAlpha = ForwardAcceleration * IndoorSpeedChange;
				}
				//moving backwards
				else
				{
					//not moving right
					if (!bIsMovingRight)
					{
						TargetForwardSpeed = BackwardSpeed * IndoorSpeedChange;
					}
					// is moving right
					else
					{
						TargetForwardSpeed = ForwardSpeed * 1.5f *IndoorSpeedChange;
					}

					ForwardLinearPhysicsAlpha = BackwardAcceleration * IndoorSpeedChange;
				}
			}
			// not moving forwards or backwards
			else
			{
				TargetForwardSpeed = 0;
				ForwardLinearPhysicsAlpha = ForwardDeceleration;
				bIsMovingForward = false;
			}

			NewVelocity = FMath::Lerp(CurrentLinearVelocity, TargetLinearVelocity, ForwardLinearPhysicsAlpha);
			ShipMesh->SetPhysicsLinearVelocity(NewVelocity, false, NAME_None);
		}
	}
}

void ASRSpaceShip::MoveRight(float value)
{
	if (!bShipOff)
	{

		CurrentRightSpeed = FMath::Lerp(CurrentRightSpeed, TargetRightSpeed*IndoorSpeedChange, RightLinearPhysicsAlpha);

		FVector CurrentLinearVelocity = ShipMesh->GetPhysicsLinearVelocity(NAME_None);

		FVector TargetLinearVelocity = GetActorRightVector() * (value*CurrentRightSpeed);

		if (value != 0)
		{
			bIsMovingRight = true;

			if (!bIsMovingForward)
			{
				TargetRightSpeed = RightSpeed * IndoorSpeedChange;
			}
			else
			{
				TargetRightSpeed = RightSpeed * 1.5f*IndoorSpeedChange;
			}

			RightLinearPhysicsAlpha = RightAcceleration * IndoorSpeedChange;
		}
		else
		{
			bIsMovingRight = false;
			TargetRightSpeed = 0;
			RightLinearPhysicsAlpha = RightDeceleration;
		}

		NewVelocity = FMath::Lerp(CurrentLinearVelocity, TargetLinearVelocity, RightLinearPhysicsAlpha);
		ShipMesh->SetPhysicsLinearVelocity(NewVelocity, false, NAME_None);
	}
}

void ASRSpaceShip::RollRight(float value)
{
	if (!bShipOff)
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
}

void ASRSpaceShip::MoveUp(float value)
{
	if (!bShipOff)
	{

		CurrentUpSpeed = FMath::Lerp(CurrentUpSpeed, TargetUpSpeed*IndoorSpeedChange, UpLinearPhysicsAlpha*IndoorSpeedChange);

		FVector CurrentLinearVelocity = ShipMesh->GetPhysicsLinearVelocity(NAME_None);

		FVector TargetLinearVelocity = GetActorUpVector() * (value*CurrentUpSpeed);

		// Moving forwards or backwards
		if (value != 0)
		{
			bIsMovingUp = true;
			//moving forwards
			if (value > 0)
			{
				// not moving right
				if (!bIsMovingRight)
				{
					TargetUpSpeed = UpSpeed * IndoorSpeedChange;
				}
				// is moving right
				else
				{
					TargetUpSpeed = UpSpeed * 1.5f *IndoorSpeedChange;
				}

				UpLinearPhysicsAlpha = UpAcceleration * IndoorSpeedChange;
			}
			//moving backwards
			else
			{
				//not moving right
				if (!bIsMovingRight)
				{
					TargetUpSpeed = DownSpeed * IndoorSpeedChange;
				}
				// is moving right
				else
				{
					TargetUpSpeed = UpSpeed * 1.5f *IndoorSpeedChange;
				}

				UpLinearPhysicsAlpha = DownAcceleration * IndoorSpeedChange;
			}
		}
		// not moving forwards or backwards
		else
		{
			TargetUpSpeed = 0;
			UpLinearPhysicsAlpha = UpDeceleration;
			bIsMovingUp = false;
		}

		NewVelocity = FMath::Lerp(CurrentLinearVelocity, TargetLinearVelocity, UpLinearPhysicsAlpha);
		ShipMesh->SetPhysicsLinearVelocity(NewVelocity, false, NAME_None);
	}
}

void ASRSpaceShip::BoostForwardPressed()
{
	if (!bShipOff)
	{
		UE_LOG(LogTemp, Warning, TEXT("BOOSTING!"));
		bIsBoosting = true;
		YawAcceleration = 0.04f;
	}

}
void ASRSpaceShip::BoostForwardReleased()
{
	if (!bShipOff)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not BOOSTING!"));
		bIsBoosting = false;
		YawAcceleration = 0.08f;
	}

}
#pragma endregion 

void ASRSpaceShip::FreeLookOn()
{
	bUseControllerRotationYaw = false;
}

void ASRSpaceShip::FreeLookOff()
{
	bUseControllerRotationYaw = true;
}