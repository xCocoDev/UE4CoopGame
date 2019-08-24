// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "CSGameState.generated.h"

/**
 * 
 */
UCLASS()
class UE4COOP_API ACSGameState : public AGameState
{
    GENERATED_BODY()


public:

    //////////////////////////////////////////////////////////////////////////
    // Writing Data

    /** Set current timer remaining of the match state */
    UFUNCTION(BlueprintCallable, Category = "Game")
    void SetTimeRemaining(const float& Time);

    /** Set the maximum amount of score the players/teams can get to */
    UFUNCTION(BlueprintCallable, Category = "Rules")
    void SetMaxScore(const int32& MaximumScore);

    /** Set maximum allowed number of rounds for this game */
    UFUNCTION(BlueprintCallable, Category = "Rules")
    void SetMaxRounds(int32 MaxValue);

    /** Set current round the gamemode is at */
    UFUNCTION(BlueprintCallable, Category = "Game")
    void SetCurrentRound(const int32& Round);

public:

    //////////////////////////////////////////////////////////////////////////
    // Reading Data

    /** Get Current Time Remaining of the match state */
    UFUNCTION(BlueprintPure, Category = "Game")
    float GetTimeRemaining() const;

    /** Get maximum amount of score a team/player can reach to win the game */
    UFUNCTION(BlueprintPure, Category = "Rules")
    int32 GetMaxScore() const;

    /** Get maximum number of rounds to play of the current game mode */
    UFUNCTION(BlueprintPure, Category = "Rules")
    int32 GetMaxRounds() const;

    /** Get current round the gamemode is at */
    UFUNCTION(BlueprintPure, Category = "Game")
    int32 GetCurrentRound() const;

    /** Whether the input for players is enabled or not */
    UFUNCTION(BlueprintPure, Category = "Game")
    bool IsInputEnabled() const;

private:

    //////////////////////////////////////////////////////////////////////////
    // Replication

    /** Maximum score a team/player can reach */
    UPROPERTY(Transient, Replicated)
    int32 MaxScore;

    /** Maximum rounds to play */
    UPROPERTY(Transient, Replicated)
    int32 MaxRounds;

    /** Current round the game mode is at */
    UPROPERTY(Transient, Replicated)
    int32 CurrentRound;

    /** Time remaining of the current match state */
    UPROPERTY(Transient, Replicated)
    float TimeRemaining;
};
