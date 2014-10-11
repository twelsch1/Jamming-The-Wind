
#pragma once

#include "JammingTheWindGameMode.generated.h"

UCLASS(minimalapi)
class AJammingTheWindGameMode : public AGameMode
{
	GENERATED_UCLASS_BODY()
	void updateScore(bool, int32);
	void setFlashLobDisc(bool);
	void setLobDiscLocation(FVector);


	UPROPERTY(EditAnywhere, Category = Spawning)
	TSubclassOf<class AActor> WhatToSpawn;

	virtual void Tick(float DeltaSeconds) OVERRIDE;

	FVector getLobDiscLocation();
	bool getFlashScoreboard();
	bool getFlashLobDisc();
	bool getGameOver();
	int32 getLeftPlayerScore();
	int32 getRightPlayerScore();
	int getLeftPlayerSetsWon();
	int getRightPlayerSetsWon();
	int getSetNumber();
	int getLIndex();

private:
	FVector lobDiscLocation;
	FVector discSpawnLocation;
	FVector leftPlayerStartLocation;
	FVector rightPlayerStartLocation;
	FRotator zeroRot;
	FRotator leftPlayerRot;
	FRotator rightPlayerRot;
	int32 leftPlayerScore;
	int32 rightPlayerScore;
	int leftPlayerSetsWon;
	int rightPlayerSetsWon;
	int lIndex;
	int setNumber;
	int setLength;
	float scoreBoardTimer;
	float scoreBoardEndTimer;
	bool flashLobDisc;
	bool flashScoreboard;
	bool spawnPlayer;
	bool lastScored;
	void scoreboardHelper(float &);
	void resetPlayersAndDisc();
	void setUpPlayers();

};



FORCEINLINE void AJammingTheWindGameMode::setFlashLobDisc(bool truth)
{
	flashLobDisc = truth;
}
FORCEINLINE bool AJammingTheWindGameMode::getFlashLobDisc()
{
	return flashLobDisc;
}
FORCEINLINE void AJammingTheWindGameMode::setLobDiscLocation(FVector vec)
{
	lobDiscLocation = vec;
}
FORCEINLINE FVector AJammingTheWindGameMode::getLobDiscLocation()
{
	return lobDiscLocation;
}
FORCEINLINE int AJammingTheWindGameMode::getLIndex()
{
	return lIndex;
}
FORCEINLINE bool AJammingTheWindGameMode::getFlashScoreboard()
{
	return flashScoreboard;
}

FORCEINLINE int32 AJammingTheWindGameMode::getLeftPlayerScore()
{
	return leftPlayerScore;
}

FORCEINLINE int32 AJammingTheWindGameMode::getRightPlayerScore()
{
	return rightPlayerScore;
}

FORCEINLINE int AJammingTheWindGameMode::getLeftPlayerSetsWon()
{
	return leftPlayerSetsWon;
}

FORCEINLINE int AJammingTheWindGameMode::getRightPlayerSetsWon()
{
	return rightPlayerSetsWon;
}

FORCEINLINE int AJammingTheWindGameMode::getSetNumber()
{
	return setNumber;
}