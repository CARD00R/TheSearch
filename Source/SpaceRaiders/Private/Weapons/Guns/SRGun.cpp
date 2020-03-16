// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Guns/SRGun.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "SpaceRaiders.h"
#include "TimerManager.h"
#include "SRCharacter.h"
#include "Components/SphereComponent.h"


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

	// Pick Up Coll
	PickUpCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PickUpCollision"));
	PickUpCollision->SetupAttachment(RootComponent);
	PickUpCollision->SetSphereRadius(180.0f);
	
	
	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";
	IsPickedUp = true;
}

void ASRGun::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60 / RateOfFire;

	// Pick Up Coll
	PickUpCollision->OnComponentBeginOverlap.AddDynamic(this, &ASRGun::OnOverlapBegin);
	PickUpCollision->OnComponentEndOverlap.AddDynamic(this, &ASRGun::OnOverlapEnd);


	if(IsPickedUp)
	{
		PickedupCollisionPreset();
	}
	else
	{
		DroppedCollisionPreset();
	}
}

void ASRGun::Fire()
{
	if (CurrentBulletsInMag > 0)
	{
		// Trace world from pawn eyes to crosshair location
		AActor* MyOwner = GetOwner();
		if (MyOwner)
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

			if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_GUN, QueryParams))
			{
				// Blocking hit! Process damage/effects

				AActor* HitActor = Hit.GetActor();
				EPhysicalSurface ObjectSurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

				float ActualDamage = BaseDamage;
				if (ObjectSurfaceType == SURFACE_CHARHEAD)
				{
					ActualDamage *= 5.0f;
				}
				else if (ObjectSurfaceType == SURFACE_CHARCHEST)
				{
					ActualDamage *= 2.0f;
				}
				if (HitActor)
				{
					UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);
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
				if (ObjectSurfaceType == SURFACE_METAL)
				{
					UE_LOG(LogTemp, Warning, TEXT("METAL"));
				}
			}

			if (DebugWeaponDrawing > 0)
			{
				//DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, 1.4f, 1.5);
			}

			PlayFireEffects(TracerEndPoint, Hit);
			LastFiredTime = GetWorld()->TimeSeconds;

			// Bullets
			if (CurrentBulletsInMag > 0)
			{
				CurrentBulletsInMag -= 1.0f;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("*click* NEED TO RELOAD"));
				//Reload
			}


		}
	}
	else
	{
		//*click click* No ammo in clip
		if (EmptyMagSFX)
		{
			FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
			UGameplayStatics::PlaySoundAtLocation(this, EmptyMagSFX, MuzzleLocation, FRotator(0, 0, 0));
		}
	}
}

void ASRGun::StartFire()
{
	if(CurrentBulletsInMag > 0)
	{
		float FirstDelay = FMath::Max(LastFiredTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

		GetWorldTimerManager().SetTimer(FireTimer, this, &ASRGun::Fire, TimeBetweenShots, true, FirstDelay);
	}
	else
	{
		//*click click* No ammo in clip
	}
}

void ASRGun::StopFire()
{
	GetWorldTimerManager().ClearTimer(FireTimer);
}

void ASRGun::PlayFireEffects(FVector TracerEnd, FHitResult HitRes)
{
	ASRCharacter* MyCharacter = Cast<ASRCharacter>(GetOwner());
	APawn* MyPawn = Cast<APawn>(GetOwner());
	FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
	
	if (MuzzleEffect)
	{

		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}
	if (TracerEffect)
	{
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
	if (FireMontage)
	{
		if(MyCharacter)
		{
			MyCharacter->PlayAnimMontage(FireMontage, 1.0f, NAME_None);	
		}
	}
	if (FireCameraShake)
	{
		// Play Camera Shake BP
		if (MyPawn)
		{
			APlayerController* PC = Cast<APlayerController>(MyPawn->GetController());
			if (PC)
			{
				PC->ClientPlayCameraShake(FireCameraShake);
			}
		}
	}
	if(FireSFX)
	{	
		UGameplayStatics::PlaySoundAtLocation(this, FireSFX, MuzzleLocation, FRotator(0, 0, 0));
	}	
}

void ASRGun::Reload()
{
	if (BulletsInReserve > 0)
	{
		float Test1 = CurrentBulletsInMag + BulletsInReserve;

		if (BulletsInReserve >= MagSize || Test1 > MagSize)
		{
			BulletsInReserve = BulletsInReserve - (MagSize - CurrentBulletsInMag);
			CurrentBulletsInMag = MagSize;
		}
		else
		{
			CurrentBulletsInMag = CurrentBulletsInMag + BulletsInReserve;
			BulletsInReserve = 0.0f;
		}
	}
	if (ReloadSFX)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		UGameplayStatics::PlaySoundAtLocation(this, ReloadSFX, MuzzleLocation, FRotator(0, 0, 0));
	}
}

void ASRGun::ReloadStart()
{
	if(BulletsInReserve > 0)
	{
		ASRCharacter* MyCharacter = Cast<ASRCharacter>(GetOwner());

		MyCharacter->SetGunStatus(EGunStatus::Egs_Reloading);

		if (MyCharacter && ReloadMontage)
		{
			MyCharacter->PlayAnimMontage(ReloadMontage, 1.0f, NAME_None);
		}
	}
}


void ASRGun::ReloadEnd()
{
	ASRCharacter* MyCharacter = Cast<ASRCharacter>(GetOwner());

	if(MyCharacter->bAimPressed)
	{
		MyCharacter->SetGunStatus(EGunStatus::Egs_ADSing);
	}
	else
	{
		MyCharacter->SetGunStatus(EGunStatus::Egs_Nis);
	}
}

void ASRGun::DroppedCollisionPreset()
{
	MeshComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	MeshComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	MeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	MeshComp->SetSimulatePhysics(true);
	
	PickUpCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickUpCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	PickUpCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PickUpCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

}
void ASRGun::PlayPickUpGunMontage()
{
	ASRCharacter* MyCharacter = Cast<ASRCharacter>(GetOwner());

	if (MyCharacter)
	{
		if(MyCharacter->GetStandingMovementStatus()==EStandingMovementStatus::Esms_Sprinting)
		{
			MyCharacter->PlayAnimMontage(PickUpGunMontage, 2.0f, NAME_None);
		}
		else if(MyCharacter->GetStandingMovementStatus() == EStandingMovementStatus::Esms_Idling)
		{
			MyCharacter->PlayAnimMontage(PickUpGunMontage, 1.4f, NAME_None);
		}
		else
		{
			MyCharacter->PlayAnimMontage(PickUpGunMontage, 1.6f, NAME_None);
		}

	}
}
void ASRGun::PlayPickUpAmmoMontage()
{
	ASRCharacter* MyCharacter = Cast<ASRCharacter>(GetOwner());

	if (MyCharacter)
	{
		if (MyCharacter->GetStandingMovementStatus() == EStandingMovementStatus::Esms_Sprinting)
		{
			MyCharacter->PlayAnimMontage(PickUpAmmoMontage, 2.0f, NAME_None);
		}
		else if (MyCharacter->GetStandingMovementStatus() == EStandingMovementStatus::Esms_Idling)
		{
			MyCharacter->PlayAnimMontage(PickUpAmmoMontage, 2.0f, NAME_None);
		}
		else
		{
			MyCharacter->PlayAnimMontage(PickUpAmmoMontage, 2.0f, NAME_None);
		}

	}
}
void ASRGun::PlayHolsterMontage()
{
	ASRCharacter* MyCharacter = Cast<ASRCharacter>(GetOwner());

	if (MyCharacter)
	{
		if (MyCharacter->GetStandingMovementStatus() == EStandingMovementStatus::Esms_Sprinting)
		{
			MyCharacter->PlayAnimMontage(HolsterMontage, -3.0f, NAME_None);
		}
		else if (MyCharacter->GetStandingMovementStatus() == EStandingMovementStatus::Esms_Idling)
		{
			MyCharacter->PlayAnimMontage(HolsterMontage, -3.0f, NAME_None);
		}
		else
		{
			MyCharacter->PlayAnimMontage(HolsterMontage, -3.0f, NAME_None);
		}

	}
}
void ASRGun::PickedupCollisionPreset()
{
	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PickUpCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickUpCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

void ASRGun::SetPickedUpState(bool NewIsPickedUp)
{
	IsPickedUp = NewIsPickedUp;
}

void ASRGun::OnOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ASRCharacter* myCharacter = Cast<ASRCharacter>(OtherActor);
	myCharacter->SetProximityGunPickUp(this);
}

void ASRGun::OnOverlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	ASRCharacter* myCharacter = Cast<ASRCharacter>(OtherActor);
	myCharacter->SetProximityGunPickUp(nullptr);
}
