// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

enum class EWaveState : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*,KillerControler ); //
/**
 * 
 */
UCLASS()
class COOPGAME_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:

	FTimerHandle TimerHandle_BotSpawner;

	// Number of bots to spawn in current wave
	int32 NrOfBotsToSpawn;

	int32 WaveCount;

	FTimerHandle TimerHandle_NextWaveStart;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBetweenWaves;

	void SpawnBotTimerElapsed();

	void StartWave();

	void EndWave();

	void PrepareForNextWave();

	void CheckWaveState();

	void CheckAnyPlayerAlive();

	void GameOver();

	void SetWaveState(EWaveState NewState);

	// Respawn the Dead Players at the start of next wave
	void RestartDeadPlayers();

public:

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
		void SpawnNewBot();

	ASGameMode();

	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnActorKilled OnActorKilled;

};
