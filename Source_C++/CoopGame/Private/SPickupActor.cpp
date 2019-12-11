// Fill out your copyright notice in the Description page of Project Settings.


#include "SPickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "TimerManager.h"
#include "SPowerUpActor.h"

// Sets default values
ASPickupActor::ASPickupActor()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = SphereComp;
	SphereComp->SetSphereRadius(75.f);

	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->SetupAttachment(RootComponent);
	DecalComp->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	DecalComp->DecalSize = FVector(64.f, 75.f, 75.f);

	CooldownDuration = 10.0f;

	SetReplicates(true);
	
}

// Called when the game starts or when spawned
void ASPickupActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (Role == ROLE_Authority) 
	{
		Respawn();
	}
	

}

void ASPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	
	if (Role == ROLE_Authority && PowerUpInstance)
	{
		PowerUpInstance->ActivatePowerUp(OtherActor);
		PowerUpInstance = nullptr;
		//UE_LOG(LogTemp, Warning, TEXT(" ACTIVATED"), *GetName());
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &ASPickupActor::Respawn, CooldownDuration);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT(" Instance failed "), *GetName());
	}


}

void ASPickupActor::Respawn()
{
	if (PowerUpClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT(" Class NULL "), *GetName());
		return;
			
	}
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	PowerUpInstance = GetWorld()->SpawnActor<ASPowerUpActor>(PowerUpClass, GetTransform());


}