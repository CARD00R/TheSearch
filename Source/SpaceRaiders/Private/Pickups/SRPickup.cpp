
// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/SRPickup.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ASRPickup::ASRPickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Mesh
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	// Collision
	PickupCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PickUpCollision"));
	PickupCollision->SetupAttachment(RootComponent);
	PickupCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	PickupCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PickupCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

// Called when the game starts or when spawned
void ASRPickup::BeginPlay()
{
	Super::BeginPlay();

	// Pick Up Coll
	PickupCollision->OnComponentBeginOverlap.AddDynamic(this, &ASRPickup::OnOverlapBegin);
	PickupCollision->OnComponentEndOverlap.AddDynamic(this, &ASRPickup::OnOverlapEnd);

}

void ASRPickup::Store()
{
	//ASRCharacter* MyCharacter = Cast<ASRCharacter>(GetOwner());
}

// Called every frame
void ASRPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASRPickup::OnOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ASRCharacter* myCharacter = Cast<ASRCharacter>(OtherActor);
	myCharacter->SetProxmityUtilityPickup(this);
}

void ASRPickup::OnOverlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	ASRCharacter* myCharacter = Cast<ASRCharacter>(OtherActor);
	myCharacter->SetProxmityUtilityPickup(nullptr);
}

EPickupType ASRPickup::GetUtilityPickupType()
{

	return PickupType;
}

void ASRPickup::ConsumeUtility()
{
	// Health and Stamina
}
