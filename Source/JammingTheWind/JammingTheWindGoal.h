

#pragma once

#include "GameFramework/Actor.h"
#include "JammingTheWindGoal.generated.h"

/**
 * 
 */
UCLASS()
class AJammingTheWindGoal : public AActor
{
	GENERATED_UCLASS_BODY()
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Goal)
	TSubobjectPtr<UBoxComponent> GoalBaseCollisionComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Goal)
	TSubobjectPtr<UStaticMeshComponent> GoalMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Goal)
	int32 GoalValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Goal)
	bool ScoringSwitch;


	
};
