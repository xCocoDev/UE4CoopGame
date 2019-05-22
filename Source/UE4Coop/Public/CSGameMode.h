// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CSGameMode.generated.h"

enum class EWaveState : uint8;

/**
 * 
 */
UCLASS()
class UE4COOP_API ACSGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:

    UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
    void SpawnNewBot();

    void SpawnBotTimerElapse();

    void StartWave();

    void EndWave();

    void GameOver();

    void PrepareForNextWave();

    void CheckWaveState();

    void CheckAnyPlayerAlive();

    void SetWaveState(EWaveState NewState);

protected:

    int WaveCount;

    int NumberOfBotsToSpawn;

    UPROPERTY(EditDefaultsOnly, Category = "GameMode")
    float TimeBetweenWaves;

    FTimerHandle TimerHandle_BotSpawner;

    FTimerHandle TimerHandle_NextWaveStart;

    FTimerHandle TimerHandle_CheckWaveState;

    FTimerHandle TimerHandle_CheckAnyPlayerAlive;

public:

    ACSGameMode();

    virtual void StartPlay() override;
};
