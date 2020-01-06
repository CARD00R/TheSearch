// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <hlslcc/hlslcc/src/hlslcc_lib/compiler.h>
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
	Ess_InAir UMETA(DisplayName = "InAir"),
	Ess_Sliding UMETA(DisplayName = "Sliding"),
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

UENUM(BlueprintType)
enum class EInAirStatus : uint8
{
	Eias_Jumping UMETA(DisplayName = "Jumping"),
	Eias_Falling UMETA(DisplayName = "Falling"),
	Eias_Flailing UMETA(DisplayName = "Flailing"),
	Eias_Nis UMETA(DisplayName = "NIS"),
	Eias_MAX UMETA(DisplayName = "DefaultMax"),
};

UENUM(BlueprintType)
enum class ESlideStatus : uint8
{
	Eias_FlatSlope UMETA(DisplayName = "FlatSlope"),
	Eias_SlantedSlope UMETA(DisplayName = "SlantedSlope"),
	Eias_SteepSlope UMETA(DisplayName = "SteepSlope"),
	Eias_NoSlope UMETA(DisplayName = "NoSlope"),
	Eias_MAX UMETA(DisplayName = "DefaultMax"),
};


UCLASS()
class SPACERAIDERS_API ASRCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASRCharacter();

	// Global Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	bool bGlobalKeysInput;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	bool bGlobalMouseInput;
	
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Components")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;
	
	APlayerController* PlayerController;

	//Capusule
	float StandingCapsuleHeight = 88.0f;
	float StandingCapsuleRadius = 20.0f;
	float CrouchingCapsuleHeight = 63.0f;
	float CrouchingCapsuleRadius = 27.0f;
	float SlidingCapsuleHeight = 30.0f;
	float SlidingCapsuleRadius = 30.0f;
	


	
	// Construction Variables
	//Mesh
	FVector MeshInitialiseLocation = FVector(-5, 0, -88);
	FRotator MeshInitialiseRotation = FRotator(0, -90, 0);
	//SpringArmComponent
	FVector SpringArmInitialiseLocation = FVector(-14, 2, 70);
	FVector SpringArmInitialiseSocketOffset = FVector(0, 90, 30);


	//InAir Status
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	EInAirStatus InAirStatus;

	// Stance Status
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	EStanceStatus StanceStatus;
	void SetStanceStatus(EStanceStatus Status);   

	//Standing Movement Status
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	EStandingMovementStatus StandingMovementStatus;
	void SetStandingMovementStatus(EStandingMovementStatus Status);

	//Crouching Movement Status
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	ECrouchingMovementStatus CrouchingMovementStatus;
	void SetCrouchingMovementStatus(ECrouchingMovementStatus Status);

	//Crouching Movement Status
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	ESlideStatus SlideStatus;
	void SetSlideStatus(ESlideStatus Status);


	EStanceStatus GetStanceStatus();
	EStandingMovementStatus GetStandingMovementStatus();
	ECrouchingMovementStatus GetCrouchingMovementStatus();
	EInAirStatus GetInAirStatus();

	void SetInAirStatus(EInAirStatus Status);
	bool GetIsArmed();
	bool GetShouldHardLand();
	//Capsule
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	bool bCheckCapsuleProperties = false;

	// Landing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fall")
	float FallHeight = 0;

	// Sliding
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	bool SlideRequest = false;
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
	//Capsule
	void CheckCapsuleHeightRadius();
	// Input Check
	void CrouchSlideCheckPressed();
	void CrouchSlideCheckReleased();

	
	// Crouch
	void CrouchToggle();
	void BeginCrouch();
	void EndCrouch();
	bool bToggleCrouch = false;

	//Movement Properties
	float JogSpeed = 600.0f;
	float BackwardsJogSpeed = 400.0f;
	float DiagonalSprintSpeed = 800.0f;
	float SprintSpeed = DefaultSprintSpeed;
	float DefaultSprintSpeed = 900.0f;
	float CrouchSpeed = 300.0f;
	void SetCharacterMovementSpeed(float MoveSpeed);
	float GetCharacterMovementSpeed();

	// Sprint
	void StartSprint();
	void SprintReleased();
	void EndSprint();
	void JustPressedSprint();
	float EndSprintDelay=1.1f;
	FTimerHandle TimerEndSprint;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bJustPressedSprint = false;
	
	// Jump
	void StartJump();

	// Slide
	void StartSlide();
	void EndSlide();
	void SlideSlopeDetection();
	void SlideSpeedCalculation();
	float SlideSpeed = 1000.0f;
	float SlideDuration = 1.5f;
	float SlideTraceLength = 1500.0f;
	bool SlideCheck = false;
	FTimerHandle TimerSlideDuration;
	FTimerHandle TimerSlopeDetection;

	// FreeLook
	void FreeLookOn();
	void FreeLookOff();

	//GlobalTimers
	FTimerHandle TimerGlobalKeysInput;
	FTimerHandle TimerGlobalMouseInput;

	// Landing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fall")
	bool bShouldHardLand;
	float LandDelay;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fall")
	float HardLandDelay = 1.7f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fall")
	float SoftLandDelay = 0.8f;
		
	//Weapons
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
	bool bIsArmed = false;

	//Input
	void GlobalKeysInputDisable();
	void GlobalKeysInputEnable();
	void GlobalMouseInputDisable();
	void GlobalMouseInputEnable();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Landed(const FHitResult& Hit) override;

};
