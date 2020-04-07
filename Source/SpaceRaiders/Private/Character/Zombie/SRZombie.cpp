// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Character/Zombie/SRZombie.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ASRZombie::ASRZombie()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Mesh
	GetMesh()->SetWorldLocationAndRotation(MeshInitialiseLocation, MeshInitialiseRotation);
	GetMesh()->SetRelativeScale3D(MeshInitialiseScale);

	// Capsule
	GetCapsuleComponent()->SetCapsuleRadius(CapsuleInitialiseRadius);
	GetCapsuleComponent()->SetCapsuleHalfHeight(CapsuleInitialiseHalfHeight);
	
}

// Called when the game starts or when spawned
void ASRZombie::BeginPlay()
{
	Super::BeginPlay();

	SetStanceStatus(EZStanceStatus::Ezss_Standing);
	SetStandingStatus(EZStandingStatus::Ezss_Running);
}

// Called every frame
void ASRZombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASRZombie::SetStanceStatus(EZStanceStatus Status)
{
	StanceStatus = Status;
}

EZStanceStatus ASRZombie::GetStanceStatus()
{
	return EZStanceStatus();
}

void ASRZombie::SetStandingStatus(EZStandingStatus Status)
{
	StandingStatus = Status;
}

EZStandingStatus ASRZombie::GetStandingStatus()
{
	return EZStandingStatus();
}

