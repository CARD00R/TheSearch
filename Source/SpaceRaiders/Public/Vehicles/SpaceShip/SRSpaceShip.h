// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SRSpaceShip.generated.h"

class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class ASRCharacter;
UCLASS()
class SPACERAIDERS_API ASRSpaceShip : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASRSpaceShip();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Components
		// Skeletal Mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* ShipMesh;
		// Spring Arm
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;
		// Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;

	//Camera Variables
	UPROPERTY(EditDefaultsOnly, Category = "Camera Variables|Rotation")
	float CameraRotationLagSpeed = 900.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Camera Variables|Rotation")
	float CameraLagSpeed = 700.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Camera Variables|Rotation")
	float CameraLagMaxDistance = 5000.0f;

	// Movement
		//Methods
	void LookUp(float value);
	void LookRight(float value);
	void MoveForward(float value);
	void MoveRight(float value);
	void RollRight(float value);
	void MoveUp(float value);
		//Variables
			// Physics
	UPROPERTY(VisibleInstanceOnly, Category = "Movement Variables|Physics")
	FVector NewVelocity = FVector(0, 0, 0);
	UPROPERTY(EditInstanceOnly, Category = "Movement Variables|Physics")
	float ForwardLinearPhysicsAlpha = 0.1f;
	UPROPERTY(EditInstanceOnly, Category = "Movement Variables|Physics")
	float RightLinearPhysicsAlpha = 0.1f;
	UPROPERTY(EditInstanceOnly, Category = "Movement Variables|Physics")
	float UpLinearPhysicsAlpha = 0.1f;
			// Camera
	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables|Rotation")
	float RotationSpeed = 500.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables|Rotation")
	float PitchAcceleration = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables|Rotation")
	float YawAcceleration = 0.1f;
			//Forward/Backward
	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables|Forward")
	float ForwardSpeed = 45000.0f;
	UPROPERTY(VisibleInstanceOnly, Category = "Movement Variables|Forward")
	float CurrentForwardSpeed = 0.0f;
	UPROPERTY(EditInstanceOnly, Category = "Movement Variables|Forward")
	float TargetForwardSpeed = 45000.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables|Forward")
	float ForwardAcceleration = 0.02f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables|Forward")
	float ForwardDeceleration = 0.008f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables|Backward")
	float BackwardSpeed = 10000.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables|Backward")
	float BackwardAcceleration = 0.007f;
			//Right
	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables|Side")
	float RightSpeed = 25000.0f;
	UPROPERTY(VisibleInstanceOnly, Category = "Movement Variables|Forward")
	float CurrentRightSpeed = 0.0f;
	UPROPERTY(EditInstanceOnly, Category = "Movement Variables|Forward")
	float TargetRightSpeed = 6500.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables|Side")
	float RightAcceleration = 0.01f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables|Side")
	float RightDeceleration = 0.004f;
			// Roll
	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables|Roll")
	float RollSpeed = 1500.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables|Roll")
	float RollAcceleration = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables|Roll")
	float RollDeceleration = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables|Roll")
	float AutoRollAmount = 0.3f;
			// Up
	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables|Forward")
		float UpSpeed = 25000.0f;
	UPROPERTY(VisibleInstanceOnly, Category = "Movement Variables|Forward")
		float CurrentUpSpeed = 0.0f;
	UPROPERTY(EditInstanceOnly, Category = "Movement Variables|Forward")
		float TargetUpSpeed = 25000.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables|Forward")
		float UpAcceleration = 0.02f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables|Forward")
		float UpDeceleration = 0.008f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables|Backward")
		float DownSpeed = 15000.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables|Backward")
		float DownAcceleration = 0.007f;
		//States
	bool bIsMovingForward;
	bool bIsMovingRight;
	bool bIsMovingUp;
	bool bIsMovingDown;

	// General Variables
	FVector ZeroVector = FVector(0, 0, 0);

	// Inventory
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	ASRCharacter* Pilot;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
