// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerUpActor.generated.h"

class URotatingMovementComponent;

UCLASS()
class COOPGAME_API ASPowerUpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerUpActor();

protected:

	UPROPERTY( VisibleDefaultsOnly,BlueprintReadWrite, Category = "PowerUps")
	UStaticMeshComponent* MeshComp;

	// Time Between Power Ups Ticks
	UPROPERTY(EditDefaultsOnly,Category = "PowerUps")
	float PowerupInterval;

	// Total Time we apply powerup Effect
	UPROPERTY(EditDefaultsOnly, Category = "PowerUps")
	int32 TotalNrOfTicks;

	UPROPERTY( VisibleDefaultsOnly,Category = "PowerUps")
	URotatingMovementComponent* RotatingMovementComp;

	// Total Number of ticks applied, tick tracker
	int32 TicksProcessed;

	FTimerHandle TimerHandle_PowerUpTick;

	UFUNCTION()
	void OnTickPowerUp();
	
	UPROPERTY(ReplicatedUsing= OnRep_PowerupActive)
	bool bIsPowerupActive;

	UFUNCTION()
	void OnRep_PowerupActive();

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void OnPowerUpStateChanged(bool bNewIsActive);

public:	

	void ActivatePowerUp(AActor* ActivateFor);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void OnActivated(AActor* ActivateFor);

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void OnPowerUpTicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void OnExpired();

	

};
