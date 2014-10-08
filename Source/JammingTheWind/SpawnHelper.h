

#pragma once

#include "GameFramework/Actor.h"
#include "SpawnHelper.generated.h"

/**
 * 
 */
UCLASS()
class ASpawnHelper : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Disc)
	FVector DiscSpawnLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Disc)
	FRotator DiscSpawnRotation;
	
	UPROPERTY(VisibleInstanceOnly, Category = Spawning)
	TSubobjectPtr<UBoxComponent> WhereToSpawn;

	UPROPERTY(EditAnywhere, Category = Spawning)
	TSubclassOf<class AActor> WhatToSpawn;

	virtual void Tick(float DeltaSeconds) OVERRIDE;

private:

	bool Check;
	void SpawnDisc();
};
