// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "CoopGame.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(TEXT("COOP.DebugWeapons"), DebugWeaponDrawing, TEXT("Draw Debug Lines For Weapons"), ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{
 	
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "Muzzle_01";
	TracerTargetName = "BeamEnd";

	BaseDamage = 20.0f;
	RateOfFire = 600;
	BulletSpread = 2.0f;
	SetReplicates(true);

	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();


	TimeBetweenShots = 60 / RateOfFire;
}


void ASWeapon::Fire()
{
	if (Role < ROLE_Authority)
	{
		ServerFire();
	}
	AActor *MyOwner = GetOwner();

	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
		FVector ShotDirection = EyeRotation.Vector();

		float HalfRad = FMath::DegreesToRadians(BulletSpread);
		ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);
		FHitResult Hit;
		FVector TracerEndPoint = TraceEnd;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		EPhysicalSurface SurfaceType = SurfaceType_Default;

		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TracerEndPoint, COLLISION_WEAPON, QueryParams))
		{
			AActor* HitActor = Hit.GetActor();
			float ActualDamage = BaseDamage;

			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			if (SurfaceType == SURFACE_FLESHVUNERABLE)
			{
				ActualDamage = 80.0f;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection ,Hit, MyOwner->GetInstigatorController(), MyOwner, DamageType);

			if (DebugWeaponDrawing > 0)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, HitActor->GetFName().ToString());
			}
					
			PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

			TracerEndPoint = Hit.ImpactPoint;
									
		}
		 

		if (DebugWeaponDrawing>0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TracerEndPoint, FColor::Black, false, 1.0f, 0, 1.0f);
		}

		PlayFireEffects(TracerEndPoint);

		if (Role == ROLE_Authority)
		{
			HitScanTrace.TraceTo = TracerEndPoint;
			HitScanTrace.SurfaceType = SurfaceType;
		}

		LastFiredTime = GetWorld()->TimeSeconds;
	}
}

void ASWeapon::ServerFire_Implementation()
{
	Fire();
}
bool ASWeapon::ServerFire_Validate()
{
	// Anti cheat
	return true;
}
void ASWeapon::OnRep_HitScanTrace()
{
	// Plat Efx
	PlayFireEffects(HitScanTrace.TraceTo);
	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo); 
}
void ASWeapon::StartFire()
{
	Fire();
	//float FirstDelay = FMath::Max(LastFiredTime + TimeBetweenShots - GetWorld()->TimeSeconds,0.f);

	//GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots,this,&ASWeapon::Fire,TimeBetweenShots, true,FirstDelay);
	
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}


void ASWeapon::PlayFireEffects(FVector TraceEnd)
{
	
	if (MuzzleEffect && TracerEffect)
	{
		MuzzleSocketName = "Muzzle_01";
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
		
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
			
		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation );
		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TracerTargetName, TraceEnd);
		}
	}

	APawn * MyOwner = Cast<APawn>(GetOwner());
	if(MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			PC->ClientPlayCameraShake(FireCamShake);
		}
	}

}




void ASWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;

	switch (SurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
		SelectedEffect = FleshImpactEffect;
		break;

	case SURFACE_FLESHVUNERABLE:
		SelectedEffect = FleshImpactEffect;
		break;

	default:
		SelectedEffect = ImpactEffect;
		break;
		
	}

	if (MeshComp == nullptr)
		return;

	if (SelectedEffect)
	{

		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation(), true);
	}
}



void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASWeapon, HitScanTrace,COND_SkipOwner);
}

