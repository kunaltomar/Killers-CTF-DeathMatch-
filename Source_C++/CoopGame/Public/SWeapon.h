// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;

USTRUCT()
struct FHitSaneTrace {
	GENERATED_BODY()
		
public:

	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
	FVector_NetQuantize TraceTo;
};

UCLASS()
class COOPGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

protected:
	virtual void BeginPlay();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem *MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem *TracerEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem *ImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem *FleshImpactEffect;

	

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Components")
	TSubclassOf<UDamageType> DamageType;

	void PlayFireEffects(FVector TraceEnd);

	void PlayImpactEffects(EPhysicalSurface SurfaceTypes, FVector ImpactPoint);

	UPROPERTY(EditDefaultsOnly,Category = "Components")
	TSubclassOf<UCameraShake> FireCamShake;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	float BaseDamage;

	virtual void Fire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	FTimerHandle TimerHandle_TimeBetweenShots;
	float LastFiredTime;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RateOfFire;

	float TimeBetweenShots;

	// Bullet Spread in degrees
	UPROPERTY(EditDefaultsOnly, Category = "Weapon",meta = (ClampMin = 0.0f))
	float BulletSpread;

	UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
	FHitSaneTrace HitScanTrace;

	UFUNCTION()
	void OnRep_HitScanTrace();

public:	
	//UFUNCTION(BlueprintCallable, Category = "Weapon")
		//virtual void Fire();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* MeshComp;

	void StartFire();
	void StopFire();


	
	
};
