

#include "JammingTheWind.h"
#include "Disc.h"
#include "SpawnHelper.h"


ASpawnHelper::ASpawnHelper(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	Check = true;
	WhereToSpawn = PCIP.CreateDefaultSubobject<UBoxComponent>(this, TEXT("WhereToSpawn"));

	RootComponent = WhereToSpawn;
	DiscSpawnLocation = FVector{ 94.0, -1040, 240.0 };

	PrimaryActorTick.bCanEverTick = true;

}


void ASpawnHelper::Tick(float DeltaSeconds)
{
	if (Check)
	{

		SpawnDisc();
	}
}

void ASpawnHelper::SpawnDisc()
{
	//spawns a disc around center of world on first tick then destroys itself.
	//this class may soon be deprecated, as it is only used once with gameMode spawning discs in
	//from that point on.
	if (Check)
	{


		UWorld* const World = GetWorld();

		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;

			DiscSpawnRotation.Yaw = 0;
			DiscSpawnRotation.Pitch = 0;
			DiscSpawnRotation.Roll = 0;



			AActor* const SpawnedDisc = World->SpawnActor<AActor>(WhatToSpawn, DiscSpawnLocation, DiscSpawnRotation, SpawnParams);
		}

		Check = false;
		Destroy();

	}
}