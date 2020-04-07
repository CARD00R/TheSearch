// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SRZombie.generated.h"

class USkeletalMeshComponent;
class UCapsuleComponent;
class USRHealthComponent;
class UCharacterMovementComponent;

UENUM(BlueprintType)
enum class EZStanceStatus : uint8
{
	Ezss_Standing UMETA(DisplayName = "Standing"),
	Ezss_Crawling UMETA(DisplayName = "Crawling"),
	Ezss_Eating UMETA(DisplayName = "Eating"),
	Ezss_Attacking UMETA(DisplayName = "Attacking"),
	Ezss_Dead UMETA(DisplayName = "Dead"),
	Ezss_Max UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EZStandingStatus : uint8
{
	Ezss_Idling UMETA(DisplayName = "Idling"),
	Ezss_Walking UMETA(DisplayName = "Walking"),
	Ezss_Running UMETA(DisplayName = "Running"),
	Ezss_Screaming UMETA(DisplayName = "Screaming"),
	Ezss_NIS UMETA(DisplayName = "NIS"),
	Ezss_Max UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class SPACERAIDERS_API ASRZombie : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASRZombie();

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USRHealthComponent* OwningHealthComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCharacterMovementComponent* CharMovementComp;
	
	//Enums
		//Stance Status
	UPROPERTY(EditAnywhere, Category = "Movement Properties|Status")
	EZStanceStatus StanceStatus;
	void SetStanceStatus(EZStanceStatus Status);
	EZStanceStatus GetStanceStatus();
		// Standing Status
	UPROPERTY(EditAnywhere, Category = "Movement Properties|Status")
	EZStandingStatus StandingStatus;
	void SetStandingStatus(EZStandingStatus Status);
	EZStandingStatus GetStandingStatus();


	// Construction Variables
	//Mesh
	FVector MeshInitialiseLocation = FVector(-15, 0, -110);
	FRotator MeshInitialiseRotation = FRotator(0, -85, 0);
	FVector MeshInitialiseScale = FVector(1.3, 1.3, 1.3);
	// Capsule
	float CapsuleInitialiseRadius = 42.0f;
	float CapsuleInitialiseHalfHeight = 113;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
