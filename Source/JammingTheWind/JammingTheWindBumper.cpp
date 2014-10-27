

#include "JammingTheWind.h"
#include "JammingTheWindBumper.h"


AJammingTheWindBumper::AJammingTheWindBumper(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	BumperBaseCollisionComponent = PCIP.CreateDefaultSubobject<UBoxComponent>(this, TEXT("BumperBaseCollComponent"));

	BumperMesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("GoalMesh"));

	RootComponent = BumperBaseCollisionComponent;


	BumperMesh->AttachTo(RootComponent);

	Midline = false;
}


