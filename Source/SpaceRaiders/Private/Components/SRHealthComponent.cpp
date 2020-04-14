// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SRHealthComponent.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
USRHealthComponent::USRHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	MaxHealth = 100;
	StartingHealth = 100;
	TeamNum = 255;
}


// Called when the game starts
void USRHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	AActor* MyOwner = GetOwner();
	if(MyOwner)
	{
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &USRHealthComponent::HandleTakeAnyDamage);
	}
	CurrentHealth = StartingHealth;
}

void USRHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if(Damage <= 0.0f)
	{
		return;
	}
	if (DamageCauser != DamagedActor && IsFriendly(DamagedActor, DamageCauser))
	{
		return;
	}
	//Update clamped Health
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
	
	UE_LOG(LogTemp, Warning, TEXT("Health Changed: %f"), CurrentHealth);

	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);
}

bool USRHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{
	if(ActorA == nullptr || ActorB == nullptr)
	{
		// Assume friendly
		return true;
	}
	USRHealthComponent* HealthCompA = Cast<USRHealthComponent>(ActorA->GetComponentByClass(USRHealthComponent::StaticClass()));
	USRHealthComponent* HealthCompB = Cast<USRHealthComponent>(ActorB->GetComponentByClass(USRHealthComponent::StaticClass()));


	if(HealthCompA == nullptr || HealthCompB == nullptr)
	{
		// Assume friendly
		return true;
	}
	return HealthCompA->TeamNum == HealthCompB->TeamNum;
}


