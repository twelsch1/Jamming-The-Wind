

#pragma once

#include "GameFramework/Actor.h"
#include "JammingTheWindBumper.generated.h"


UCLASS()
class AJammingTheWindBumper : public AActor
{
	GENERATED_UCLASS_BODY()
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Goal)
	TSubobjectPtr<UBoxComponent> BumperBaseCollisionComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Goal)
		TSubobjectPtr<UStaticMeshComponent> BumperMesh;
	
	
};
