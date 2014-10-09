

#include "JammingTheWind.h"
#include "JammingTheWindGameMode.h"
#include "JammingTheWindScoreboard.h"
#include "JammingTheWindCharacter.h"
#include "SpawnHelper.h"
#include "JammingTheWindPlayerController.h"

AJammingTheWindGameMode::AJammingTheWindGameMode(const class FPostConstructInitializeProperties& PCIP) : Super(PCIP)
{
	
	flashScoreboard = false;
	flashLobDisc = false;

	spawnPlayer = true;
	lastScored = true;
	leftPlayerScore = 0;
	rightPlayerScore = 0;
	leftPlayerSetsWon = 0;
	rightPlayerSetsWon = 0;
	setNumber = 1;
	setLength = 12;
	scoreBoardTimer = -1;
	scoreBoardEndTimer = 3;
	lIndex = 0;

	lobDiscLocation = { 0, 0, 0 };
	discSpawnLocation = { 82.399, -1034, 240 };
	leftPlayerStartLocation = {51.711914, -2309.08252, 294.14994};
	rightPlayerStartLocation = { 51.711914, 160.01745, 294.14994 };
	zeroRot = { 0, 0, 0 };
	leftPlayerRot = { 0, 90, 0 };
	rightPlayerRot = { 0, -90, 0 };
	PlayerControllerClass = AJammingTheWindPlayerController::StaticClass();
	HUDClass = AJammingTheWindScoreboard::StaticClass();
	
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FObjectFinder<UClass> PlayerPawnBPClass(TEXT("Class'/Game/Blueprints/MyCharacter.MyCharacter_C'"));
	if (PlayerPawnBPClass.Object != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Object;

	}
	

	
}



void AJammingTheWindGameMode::Tick(float DeltaSeconds)
{
	if (spawnPlayer)
	{
		setUpPlayers();
	}
	
	if (flashScoreboard)
	{

		scoreboardHelper(DeltaSeconds);

	}

}
void AJammingTheWindGameMode::updateScore(bool whoScored, int32 ptValue)
{

	if (!flashScoreboard)
	{

	flashLobDisc = false;
	lastScored = whoScored;
	
		if (lastScored)
			leftPlayerScore += ptValue;
		else
			rightPlayerScore += ptValue;
		flashScoreboard = true;

		TArray<AActor*> foundActors;
		int index = 0;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AJammingTheWindCharacter::StaticClass(), foundActors);
		for (auto Actor : foundActors)
		{
			AJammingTheWindCharacter* player = Cast<AJammingTheWindCharacter>(Actor);

			if (player)
			{
				player->resetHelper();
				if (!lastScored && index == 1)
				{
					player->setMovementLocked(true);
					lIndex = 1;
				}
				if (lastScored && index == 0)
				{
					player->setMovementLocked(true);
					lIndex = 0;
				}



				index++;
			}
		}

		scoreBoardTimer = 0;

	}
}
void AJammingTheWindGameMode::scoreboardHelper(float &DeltaSeconds)
{
	if (scoreBoardTimer >= 0 && scoreBoardTimer < scoreBoardEndTimer)
		scoreBoardTimer += DeltaSeconds;


	else
	{
		if (leftPlayerScore >= setLength)
		{
			setNumber++;
			leftPlayerSetsWon++;
			rightPlayerScore = 0;
			leftPlayerScore = 0;
		}
		if (rightPlayerScore >= setLength)
		{
			setNumber++;
			rightPlayerSetsWon++;
			rightPlayerScore = 0;
			leftPlayerScore = 0;
		}
		if (rightPlayerSetsWon >= 2 || leftPlayerSetsWon >= 2)
		{
			setNumber = 1;
			rightPlayerSetsWon = 0;
			leftPlayerSetsWon = 0;
		}
	
		resetPlayersAndDisc();
		flashScoreboard = false;
		scoreBoardTimer = -1;
	}
	
}
void AJammingTheWindGameMode::resetPlayersAndDisc()
{
	TArray<AActor*> foundActors;
	int index = 0;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AJammingTheWindCharacter::StaticClass(), foundActors);
	for (auto Actor : foundActors)
	{
		AJammingTheWindCharacter* player = Cast<AJammingTheWindCharacter>(Actor);

		if (player)
		{

			if (!lastScored && index == 1)
			{
				player->setMovementLocked(true);
				lIndex = 1;
			}
			if (lastScored && index == 0)
			{
				player->setMovementLocked(true);
				lIndex = 0;
			}

			if (player->getYDir() == 1)
				player->TeleportTo(leftPlayerStartLocation, leftPlayerRot);
			else
				player->TeleportTo(rightPlayerStartLocation, rightPlayerRot);
			
			

			
			index++;
		}
	}
	
	//spawn disc in center
	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this;
	spawnParams.Instigator = Instigator;

	UWorld* const World = GetWorld();
	
	AActor* const SpawnedDisc = World->SpawnActor<AActor>(WhatToSpawn, discSpawnLocation, zeroRot, spawnParams);
	
}
void AJammingTheWindGameMode::setUpPlayers()
{
	int index = 0;
	UGameplayStatics::CreatePlayer(this, 1);
	spawnPlayer = false;
	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AJammingTheWindCharacter::StaticClass(), foundActors);
	
	for (auto Actor : foundActors)
	{
		AJammingTheWindCharacter* player = Cast<AJammingTheWindCharacter>(Actor);

		if (player)
		{
			if (index == 0)
			{
				player->ReceiveDisc = true;
				player->setMovementLocked(true);
				player->setYDir(-1);
			}
			else
			{
				player->TeleportTo(leftPlayerStartLocation, leftPlayerRot);
				player->setYDir(1);
			}
			
			index++;
		}
	}
}