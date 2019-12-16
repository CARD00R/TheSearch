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
enum class EStanceStatus : uint8
{
	Ess_Standing UMETA(DisplayName = "Standing"),
	Ess_Crouching UMETA(DisplayName = "Crouching"),
	Ess_Max UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EStandingMovementStatus : uint8
{
	Esms_Idling UMETA(DisplayName = "Idling"),
	Esms_Jogging UMETA(DisplayName = "Jogging"),
	Esms_Sprinting UMETA(DisplayName = "Sprinting"),
	Esms_Nis UMETA(DisplayName = "NIS"),
	Esms_MAX UMETA(DisplayName = "DefaultMax")


};

UENUM(BlueprintType)
enum class ECrouchingMovementStatus : uint8
{
	Ecms_Idling UMETA(DisplayName = "Idling"),
	Ecms_Walking UMETA(DisplayName = "Walking"),
	Ecms_Nis UMETA(DisplayName = "NIS"),
	Ecms_MAX UMETA(DisplayName = "DefaultMax"),

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

	// Stance Status
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	EStanceStatus StanceStatus;
	FORCEINLINE void SetStanceStatus(EStanceStatus Status);
	
	//Standing Movement Status
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	EStandingMovementStatus StandingMovementStatus;
	FORCEINLINE void SetStandingMovementStatus(EStandingMovementStatus Status);

	//Crouching Movement Status
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	ECrouchingMovementStatus CrouchingMovementStatus;
	FORCEINLINE void SetCrouchingMovementStatus(ECrouchingMovementStatus Status);
	
	
	//Movement Properties
	float JogSpeed = 600.0f;
	float DiagonalSprintSpeed = 735.0f;
	float SprintSpeed = DefaultSprintSpeed;
	float DefaultSprintSpeed = 900.0f;

	// Sprint
	void StartSprint();
	void EndSprint();
	
	// FreeLook
	void FreeLookOn();
	void FreeLookOff();

	//Weapons
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
	bool bIsArmed = false;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE EStanceStatus GetStanceStatus();
	FORCEINLINE EStandingMovementStatus GetStandingMovementStatus();
	FORCEINLINE ECrouchingMovementStatus GetCrouchingMovementStatus();
	FORCEINLINE bool GetIsArmed();
};
