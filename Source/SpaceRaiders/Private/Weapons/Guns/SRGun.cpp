// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Guns/SRGun.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "SpaceRaiders.h"
#include "TimerManager.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("SR.DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draw Debug Lines for weapons"),
	ECVF_Cheat);

// Sets default values
ASRGun::ASRGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";
}

void ASRGun::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60 / RateOfFire;
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
		
		FVector ShotDirection = EyeRotation.Vector();
		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);
		
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		FVector TracerEndPoint = TraceEnd;
		
		FHitResult Hit;
		
		if(GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_GUN, QueryParams))
		{
			// Blocking hit! Process damage/effects

			AActor* HitActor = Hit.GetActor();
			EPhysicalSurface ObjectSurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float ActualDamage = BaseDamage;
			if(ObjectSurfaceType == SURFACE_CHARHEAD)
			{
				ActualDamage *= 5.0f;
			}
			else if(ObjectSurfaceType == SURFACE_CHARCHEST)
			{
				ActualDamage *= 3.0f;
			}
			if(HitActor)
			{
				UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage,ShotDirection,Hit,MyOwner->GetInstigatorController(),this,DamageType);
			}
			
			// Obtains surface type from hit object


			switch (ObjectSurfaceType)
			{
			
			case SURFACE_CHARDEFAULT:
				SelectedImpactEffect = CharImpactEffect;
				break;
			case SURFACE_CHARCRITICAL:
				SelectedImpactEffect = CharCritImpactEffect;
				break;
			case SURFACE_CHARHEAD:
				SelectedImpactEffect = CharHeadImpactEffect;
				break;
			case SURFACE_CHARCHEST:
				SelectedImpactEffect = CharImpactEffect;
				break;
			case SURFACE_METAL:
				SelectedImpactEffect = MetalImpactEffect;
				break;
			}
			
			TracerEndPoint = Hit.ImpactPoint;
			if(ObjectSurfaceType == SURFACE_METAL)
			{
				UE_LOG(LogTemp, Warning, TEXT("METAL"));
			}
		}
			
		if(DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, 1.4f, 1.5);
		}
		
		PlayFireEffects(TracerEndPoint, Hit);
		LastFiredTime = GetWorld()->TimeSeconds;
	}
}

void ASRGun::StartFire()
{
	float FirstDelay = FMath::Max(LastFiredTime + TimeBetweenShots - GetWorld()->TimeSeconds,0.0f);
	
	GetWorldTimerManager().SetTimer(FireTimer,this, &ASRGun::Fire, TimeBetweenShots, true, FirstDelay);
}

void ASRGun::StopFire()
{
	GetWorldTimerManager().ClearTimer(FireTimer);
}

void ASRGun::PlayFireEffects(FVector TracerEnd, FHitResult HitRes)
{
	if (MuzzleEffect)
	{

		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}
	if (TracerEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);

		//Getting Tracer Particle system and setting the target parameter to the end of the tracerpoint
		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TracerTargetName, TracerEnd);
		}
	}
	if (SelectedImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedImpactEffect, HitRes.ImpactPoint, HitRes.ImpactNormal.Rotation());
	}

	// Play Camera Shake BP
	APawn* MyOwner = Cast<APawn>(GetOwner());
	if(MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if(PC)
		{
			PC->ClientPlayCameraShake(FireCameraShake);
		}
	}
}



