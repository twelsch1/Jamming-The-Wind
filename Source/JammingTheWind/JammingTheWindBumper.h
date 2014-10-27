

#pragma once

#include "GameFramework/Actor.h"
#include "JammingTheWindBumper.generated.h"


UCLASS()
class AJammingTheWindBumper : public AActor
{
	GENERATED_UCLASS_BODY()
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Bumper)
	TSubobjectPtr<UBoxComponent> BumperBaseCollisionComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Bumper)
		TSubobjectPtr<UStaticMeshComponent> BumperMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bumper)
		bool Midline;
	
};
