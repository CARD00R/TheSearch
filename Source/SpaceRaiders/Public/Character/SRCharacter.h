// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SRCharacter.generated.h"

class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class APlayerController;

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Idling UMETA(DisplayName = "Idling"),
	EMS_Jogging UMETA(DisplayName = "Jogging"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Death UMETA(DisplayName = "Dead"),
	EMS_MAX UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class SPACERAIDERS_API ASRCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASRCharacter();

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Components")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;

	APlayerController* PlayerController;
	
	// Construction Variables
	//Mesh
	FVector MeshInitialiseLocation = FVector(0, 0, -88);
	FRotator MeshInitialiseRotation = FRotator(0, -90, 0);
	//SpringArmComponent
	FVector SpringArmInitialiseLocation = FVector(0, 50, 100);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Input KB
	void MoveForward(float value);
	bool bIsMovingForward;
	void MoveRight(float value);
	bool bIsMovingRight;
	// Input Mouse
	void LookUp(float value);
	void Turn(float value);
	
	// Crouch
	void CrouchToggle();
	void BeginCrouch();
	void EndCrouch();
	bool bToggleCrouch = false;

	//Movement Status
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	EMovementStatus MovementStatus;
	FORCEINLINE void SetMovementStatus(EMovementStatus Status);

	//Movement Properties
	float JogSpeed = 450.0f;
	float SprintSpeed = 750.0f;

	// Sprint
	void StartSprint();
	void EndSprint();
	
	// FreeLook
	void FreeLookOn();
	void FreeLookOff();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
