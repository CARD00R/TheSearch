// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Guns/SRGun.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"


// Sets default values
ASRGun::ASRGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
}

// Called when the game starts or when spawned
void ASRGun::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASRGun::Fire()
{
	// Trace world from pawn eyes to crosshair location

	AActor* MyOwner = GetOwner();
	if(MyOwner)
	{
		// Gets the location and rotation of the pawn's eyes
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector TraceEnd = EyeLocation + (EyeRotation.Vector() * 10000);
		
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;

		FHitResult Hit;
		
		if(GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility, QueryParams))
		{
			// Blocking hit! Process damage/effects
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, 1.4f, 1.5);
		}
	}
}

// Called every frame
void ASRGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

