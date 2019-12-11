// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerUpActor.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASPowerUpActor::ASPowerUpActor()
{
	PowerupInterval = 0.f;
	TotalNrOfTicks = 0;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	//MeshComp->SetRelativeLocation(FVector(0, 0, 80.f));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	 
	RotatingMovementComp = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovementComp"));

	SetReplicates(true);

	bIsPowerupActive = false;

}



void ASPowerUpActor::OnTickPowerUp()
{
	TicksProcessed++;

	OnPowerUpTicked();

	UE_LOG(LogTemp, Warning, TEXT(" OnPowerUpTicked "), *GetName());

	if (TicksProcessed >= TotalNrOfTicks )
	{
		OnExpired();

		bIsPowerupActive = false;
		OnRep_PowerupActive();

		// Stop Tick timer
		GetWorldTimerManager().ClearTimer(TimerHandle_PowerUpTick);
	}

}



void ASPowerUpActor::OnRep_PowerupActive()
{
	OnPowerUpStateChanged(bIsPowerupActive);
	if (bIsPowerupActive)
	{
		// Play effects and sounds here
	}

}


void ASPowerUpActor::ActivatePowerUp(AActor* ActivateFor)
{

	OnActivated(ActivateFor);

	bIsPowerupActive = true;
	OnRep_PowerupActive();

	if (PowerupInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerUpTick, this, &ASPowerUpActor::OnTickPowerUp,0.5f, true,0.0f);
		UE_LOG(LogTemp, Warning, TEXT(" Ticked failed "), *GetName());
	}
	else
	{
		OnTickPowerUp();
	}
}

void ASPowerUpActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPowerUpActor, bIsPowerupActive);
}