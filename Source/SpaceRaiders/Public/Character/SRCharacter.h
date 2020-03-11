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
class ASRGun;
class USRHealthComponent;

UENUM(BlueprintType)
enum class EStanceStatus : uint8
{
	Ess_Standing UMETA(DisplayName = "Standing"),
	Ess_Crouching UMETA(DisplayName = "Crouching"),
	Ess_InAir UMETA(DisplayName = "InAir"),
	Ess_Sliding UMETA(DisplayName = "Sliding"),
	Ess_Dead UMETA(DisplayName = "Dead"),
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
	Ess_FlatSlope UMETA(DisplayName = "FlatSlope"),
	Ess_SlantedSlope UMETA(DisplayName = "SlantedSlope"),
	Ess_SteepSlope UMETA(DisplayName = "SteepSlope"),
	Ess_NoSlope UMETA(DisplayName = "NoSlope"),
	Ess_MAX UMETA(DisplayName = "DefaultMax"),
};

UENUM(BlueprintType)
enum class EGunStatus : uint8
{
	Egs_ADSing UMETA(DisplayName = "ADSing"),
	Egs_Reloading UMETA(DisplayName = "Reloading"),
	Egs_Nis UMETA(DisplayName = "NIS"),
	Egs_MAX UMETA(DisplayName = "DefaultMax"),
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USRHealthComponent* OwningHealthComp;
	
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
	FVector SpringArmInitialiseSocketOffset = FVector(0, 55, 30);


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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	EGunStatus GunStatus;
	void SetGunStatus(EGunStatus Status);

	EStanceStatus GetStanceStatus();
	EStandingMovementStatus GetStandingMovementStatus();
	ECrouchingMovementStatus GetCrouchingMovementStatus();
	EInAirStatus GetInAirStatus();
	EGunStatus GetGunStatus();

	void SetInAirStatus(EInAirStatus Status);
	bool GetGunHolstered();
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

	// Aiming
	float Pitch;
	float Yaw;
	
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
	float WalkSpeed = 400;
	void SetCharacterMovementSpeed(float MoveSpeed);
	float GetCharacterMovementSpeed();


	// Sprint
	void StartSprint();
	void SprintReleased();
	void EndSprint();
	void JustPressedSprint();
	float EndSprintDelay=0.7f;
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
	float SlideSpeed = 1450.0f;
	float SlideDuration = 1.5f;
	float SlideTraceLength = 1500.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	bool SlideCheck = false;
	FTimerHandle TimerSlideDuration;
	FTimerHandle TimerSlopeDetection;
	float SlowSpeedGain = 1.003f;
	float FastSpeedGain = 1.01f;
	float SlowSpeedLoss = 0.985f;
	float FastSpeedLoss = 0.970f;


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

	//Aim/Weapons
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bGunHolstered = false;
	void AimPressed();

	void AimReleased();
	void EquipPrimaryWeapon();
	void EquipSecondaryWeapon();
	float ADSCameraFOV = 60.0f;
	float WhipCameraFOV = 90.0f;
	void SetCameraFOV(float DeltaTime);
	float ZoomInterpSpeed = 7.0f;
	bool bChangeFOV = false;
	void PullTrigger();
	void ReleaseTrigger();
	UPROPERTY(EditDefaultsOnly, Category= "Weapon")
	TSubclassOf<ASRGun> StarterWeaponClass;
	UPROPERTY(VisibleDefaultsOnly, Category= "Weapon")
	FName WeaponAttachSocketName = "StarterGunSocket";
	FName WeaponHolsterSocketName = "HolsterPistolSocket";
	void ReloadRequest();
	void Reload();
	void DropWeapon();
	void PickUpWeapon(ASRGun* WeaponToPickUp);
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Weapon")
	ASRGun* ProximityGunPickUp;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Weapon")
	ASRGun* PrimaryWeapon;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Weapon")
	ASRGun* SecondaryWeapon;
	
	//Input
	void GlobalKeysInputDisable();
	void GlobalKeysInputEnable();
	void GlobalMouseInputDisable();
	void GlobalMouseInputEnable();

	// Interact
	void InteractWith();

	// Health
	UFUNCTION()
	void OnHealthChanged(USRHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual FVector GetPawnViewLocation() const override;
	// Play Montage
	float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName);
	
	// Weapon
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Weapon")
	ASRGun* EquippedWeapon;
	void SetProximityGunPickUp(ASRGun* Gun);
	bool bAimPressed = false;
};
