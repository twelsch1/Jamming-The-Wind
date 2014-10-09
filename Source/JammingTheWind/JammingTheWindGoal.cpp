

#include "JammingTheWind.h"
#include "JammingTheWindGoal.h"


AJammingTheWindGoal::AJammingTheWindGoal(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	GoalBaseCollisionComponent = PCIP.CreateDefaultSubobject<UBoxComponent>(this, TEXT("GoalBaseCollComponent"));

	GoalMesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("GoalMesh"));

	RootComponent = GoalBaseCollisionComponent;

	GoalValue = 5;

	GoalMesh->AttachTo(RootComponent);
}


