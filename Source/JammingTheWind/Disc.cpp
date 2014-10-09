

#include "JammingTheWind.h"
#include "JammingTheWindBumper.h"
#include "JammingTheWindCharacter.h"
#include "JammingTheWindGameMode.h"
#include "JammingTheWindGoal.h"
#include "Disc.h"



ADisc::ADisc(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	SpeedFactor = 1.0;
	BaseCollisionComponent = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("BaseSphereComponent"));


	RootComponent = BaseCollisionComponent;
	DiscMesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("DiscMesh"));

	

	DiscMesh->AttachTo(RootComponent);
	PrimaryActorTick.bCanEverTick = true;
	Spawned = true;

	StandardDirection = { 1, 1, 0 };
	lobDelta = { 0, 0, 0 };

	isStandard = false;
	isCurved = false;
	isSpecial = false;
	isLobShot = false;
	aimDirLeft = false;
	//curveRebound = false;
	playerPossessing = false;
	
	//used to "graph" out parabolic functions in lob and curve shots
	//as well as the linear movement of standard shot
	curveHelper = 0;

	//used for the flip/block event
	flipTimer = -1;
	flipOverTime = 1.5;
	flipping = false;

	//used to make sure the disc doesn't get "trapped" in the bumper
	//when contact with bumper is made, it won't check for another contact for 0.2 seconds.
	bumperCollTimer = -1;
	bumpCollEndTimer = 0.2;


	/*these coordinates are based off positions on map established in editor
	Many of these variables are also in the character class. In a multimap version
	of this game, I would likely have these variables in gameMode and then have
	the classes call gameMode when they need them. That way gameMode could get different
	variables dependent on the map and abstract this from the character and
	disc classes. For now, to keep the code clean of a bunch of function calls 
	to gameMode, I have the variables in both classes. */
	yMin = -2510;
	yMax = 430;
	outOfBoundsOffset = 10;
	xMin = -802.601074;
	xMax = 967.398926;
	midLine = -1040;
	xOffset = 160; //was 160, 140 looked pretty good

	//used in crossed midline helper
	crossedMidline = true;
	midlineTimer = -1;
	midlineMinTime = 0.1;


	

	//modify to gameplay feel
	movementPerSecond = 2500; //default value
	bumperSpeedUp = 0; //currently bumper does not speed or slow down disc at all
	minYDistance = 600; //ensure that disc has to move toward opposite goal, even if player shoots straight up
	curveOffset = 600; //used in parabolic equation
	zOffset = 240; //z position in space where disc sits 
}

void ADisc::Tick(float DeltaSeconds)
{
	
	Super::Tick(DeltaSeconds);

	if (Spawned)
	{
	//on first tick, get game mode, establish loser, send disc to loser using spawn move
	myGameMode = Cast<AJammingTheWindGameMode>(UGameplayStatics::GetGameMode(this));
	LoserIndex = myGameMode->getLIndex(); 
	SpawnMove();
	//close the gate behind us as we enter
	Spawned = false;
	}
	
	//run our timers which support various functions
	runDiscTimers(DeltaSeconds);

	//if shot is true, execute shot for this frame
	if (isStandard)
		standardShot(DeltaSeconds);

	if (isLobShot)
		lobShot(DeltaSeconds);
		
	if (isSpecial)
		specialShot(DeltaSeconds);
	
	if (isCurved)
	{
		//check if first run through, if so set curve distance 
		if (curveFirstRun)
		{
			calculateCurveDistance();
			curveFirstRun = false;
		}

		curvedShot(DeltaSeconds);
	}

	
}



void ADisc::SpawnMove_Implementation()
{
	
	//empty function, actual implementation handled with blueprints.
	//The blueprint simply uses the move component to function to move the disc to player 1 at the start of the game
	//and to the player who last lost a point for every proceeding spawn. I could have done this in code fairly easily
	//but I was experimenting with blueprint early on and kept this function since it worked.
}

void ADisc::flip(float &DeltaSeconds)
{
	/*flips the disc like a coin by altering its roll and sending the disc up until it reaches
	halfway point of timer, at which point it falls at the same rate that it rose*/
	float zDistance;
	FVector Delta;
	int rps = 2; //rotations per second
	
	FRotator rot(0, 0, -(360/flipOverTime) * rps * DeltaSeconds);
	zDistance = 1000;
	//rises until it hits this point, then it starts falling down
	if (flipTimer > flipOverTime / 2) 
		zDistance *= -1;

	Delta.Set(0, 0, zDistance / flipOverTime * DeltaSeconds);
	
	RootComponent->MoveComponent(Delta, RootComponent->GetComponentRotation() + rot, true);	

}

void ADisc::lobShot(float &DeltaSeconds)
{
	/*
	Lob Shot uses a parabolic equation on z to create the lob effect while linearly moving the disc over x and y
	based off pre-calculated values. The yaw is altered to give the disc a spinning effect.
	In other places in the code, we use the pre calculated values to project the disc's final location and then
	draw a flashing circle at this spot.
	*/
	float updates, xDistance, yDistance, distance, zPeak, slowFactor, transform, f1, f2, step, lobOvertime;
	float xVal, yVal;
	int rps = 4; //rotations per second
	FVector Delta;
	//updates = 100;
	updates = 1;
	FRotator rot(0, 360 * rps * (DeltaSeconds / updates), 0);
	
	slowFactor = 2.5;
	zPeak = 1000;
	
	/*This tranform is close but not quite at one of the 0's of our quadratic equation.
	The inpreciseness is due to the disc not neccesarily colliding when it hits 0 (e.g it generally hits a little earlier), 
	so the transform was massaged until it seemed to match with the disc's final projected location.*/
	transform = 56;


	xDistance = lobDelta.X;
	yDistance = lobDelta.Y;
	distance = FMath::Sqrt(FMath::Square(xDistance) + FMath::Square(yDistance));
	lobOvertime = distance * slowFactor / movementPerSecond;

	for (int i = 0; i < updates; ++i)
	{
		
		f1 = -1 * FMath::Square(((curveHelper/lobOvertime * transform) - FMath::Sqrt(zPeak - zOffset))) + zPeak;
		step = curveHelper + (DeltaSeconds / updates);
		f2 = -1 * FMath::Square(((step / lobOvertime * transform) - FMath::Sqrt(zPeak - zOffset))) + zPeak;
		curveHelper = step;
	
		yVal = yDistance / lobOvertime * (DeltaSeconds / updates);
		xVal = xDistance / lobOvertime * (DeltaSeconds / updates);

		Delta.Set(xVal, yVal , f2-f1);

		RootComponent->MoveComponent(Delta, RootComponent->GetComponentRotation() + rot, false);

		//when player comes into possession and sets this value to true, we kill the shot
		if (playerPossessing)
		{
			isLobShot = false;
			curveHelper = 0;
		
			
			
			myGameMode->setFlashLobDisc(false);
			return;
		}
		
			//check if it's made contact with the ground, which is a goal of value 2
			checkGoalCollision();
	}
	
}
void ADisc::standardShot(float &DeltaSeconds)
{
	/* A shot that moves linearly across x and y planes. Distance/direction is determined by
	where analog stick is aimed. Currently has 0 rotation.
	*/
	float overTime, updates, endLoop, xDistance, yDistance, distance;
	FVector Delta;
	int rps = 0; //rotations per second

//	updates = 100;
	updates = 1;
	FRotator rot(0, 360 * rps * (DeltaSeconds / updates) * StandardDirection.X, 0);
	
	if (yDirection < minYDistance)
		yDistance = minYDistance * StandardDirection.Y;
	else
		yDistance = yDirection * StandardDirection.Y;
	
	xDistance = xDirection;
	distance = FMath::Sqrt(FMath::Square(xDistance) + FMath::Square(yDistance));
	overTime = distance / movementPerSecond;
	//code that I might add back in, would make it so rebound off curve shot would be predictable
	/*if (curveRebound)
	{
		yDistance = 2000 * StandardDirection.Y;
		xDistance = -2000 * StandardDirection.X;
		yDirection = yDistance;
		xDirection = xDistance;
	}*/
	endLoop = curveHelper + DeltaSeconds;
	while (curveHelper < endLoop)
	{
	
		
		curveHelper += (DeltaSeconds / updates);
		Delta.Set(xDistance / overTime * (DeltaSeconds/updates), yDistance / overTime * (DeltaSeconds/updates), 0);

		RootComponent->MoveComponent(Delta, RootComponent->GetComponentRotation() + rot, false);
		
		if (bumperCollision() && bumperCollTimer < 0)
		{
			xDirection *= -1;
		
			bumperCollTimer = 0;
			StandardDirection.Set(StandardDirection.X * -1, StandardDirection.Y, 0);
			movementPerSecond += bumperSpeedUp;
			//curveRebound = false;
		}
	
		if (playerPossessing)
		{
			isStandard = false;
			//curveRebound = false;
			curveHelper = 0;
			
			return;
		}
		checkGoalCollision();

	}
}


void ADisc::specialShot(float &DeltaSeconds)
{
	/* The shot advances along one axis until it hits the wall, at which point it advances along the other, speeding up as it goes.
	Spins in the direction it is shot.
	*/
	float xDistance, yDistance,  overTime, speedUp;
	bool helperBool;
	int rps = 3;
	FVector Delta;
	
	FRotator rot(0, 360 * rps * (DeltaSeconds) * StandardDirection.X, 0);
	speedUp = 2000;
	xDistance = 4000;
	yDistance = 0;
	//I've considered having a seperate MovementPerSecond for charge, which would be equal to MPS except would have a higher minimum
	//and lower maximum and would check to see if it was within those bounds. Thus, why there is a chargeMPS commented out below.
	//chargeMPS = 3000;
	overTime = xDistance / movementPerSecond;
	xDistance *= StandardDirection.X;
	//check if it is near the wall
	helperBool = (StandardDirection.X == 1 && RootComponent->GetComponentLocation().X >= xMax - xOffset) ||
		(StandardDirection.X == -1 && RootComponent->GetComponentLocation().X <= xMin + xOffset);
	if (helperBool)
	{
		xDistance *= 0;
		yDistance = 4000 * StandardDirection.Y;
		movementPerSecond += speedUp * DeltaSeconds;

	}
		
	Delta.Set(xDistance / overTime * DeltaSeconds , yDistance / overTime * DeltaSeconds, 0);

	RootComponent->MoveComponent(Delta, RootComponent->GetComponentRotation() + rot, true);
		
	if (helperBool)
	{
		if (playerPossessing)
		{
			isSpecial = false;
			return;
		}
	}
	

		checkGoalCollision();
	

}

void ADisc::curvedShot(float &DeltaSeconds)
{
	/* Shot is curved along a parabolic graph, spinning as it goes.
	*/

	FVector Delta;
	float f1, f2, f3, f4, step;
	float overTime, y, a, endLoop;
	int updates;
	//updates = 100;
	updates = 1;
	y = 3000; //was 3000
	a = 0.0004; //was 0.0003, 0.0005 looked interesting as well
	/*instead of doing the below each time, we calculate on the first run
	f5 = (a * FMath::Square(y - curveOffset));
	curveDistance = FMath::Sqrt(FMath::Square(f5) + FMath::Square(y - curveOffset)); 
	*/

	FRotator rot(0, 360 * 2 * (DeltaSeconds / updates) * StandardDirection.X, 0);
	overTime = curveDistance / movementPerSecond;
	

	
	endLoop = curveHelper + DeltaSeconds;
	while (curveHelper < endLoop)
	{

	
		
		f1 = StandardDirection.Y * curveOffset - StandardDirection.Y * curveHelper * y / overTime;
		step = curveHelper + (DeltaSeconds / updates);
		f2 = StandardDirection.Y * curveOffset - StandardDirection.Y * step * y / overTime;
		f3 = (StandardDirection.X * a * (f1 * f1));
		f4 = (StandardDirection.X * a * (f2 * f2));
		Delta.Set(f4 - f3, f1 - f2, 0);

		RootComponent->MoveComponent(Delta, RootComponent->GetComponentRotation() + rot, true);
		curveHelper = step;
	

		if (bumperCollision() && bumperCollTimer < 0)
		{
			bumperCollTimer = 0;
				
			StandardDirection.Set(StandardDirection.X * -1, StandardDirection.Y, 0);

			//curveRebound = true;
			movementPerSecond += bumperSpeedUp;

		}
		
		if (playerPossessing)
		{
			curveHelper = 0;
			isCurved = false;

			return;
		}
		checkGoalCollision();
			//Note: It is implicit to the design of the shot that the curved trajectory
			//will not end until it has made contact either with a player, a bumper, or a goal.
		
	}
	
}
void ADisc::calculateCurveDistance()
{
	float a, f, y;
	y = 3000;
	a = 0.0004;

	f = ( a * FMath::Square(y-curveOffset));
	curveDistance = FMath::Sqrt(FMath::Square(f) + FMath::Square(y-curveOffset)); //distance formula
	
}



bool ADisc::bumperCollision()
{
	/*
	Go through all overlapping actors, if one is a bumper then we return true.
	*/

	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for (int i = 0; i < OverlappingActors.Num(); ++i)
	{
		AJammingTheWindBumper* const myBumper = Cast<AJammingTheWindBumper>(OverlappingActors[i]);

		if (myBumper)
		{

			return true;
		}

	}

	/*Code that accomplishes this based off location rather than collision.
	While feasible, it would need to be supplemented with similar logic for restricting the player.
	Collision is the nicer way to accomplish this objective, but the collision currently in the
	game is finicky in part due to the various overlaps that are allowed. Currently, the player can walk into
	part of a wall and goal, and this also restricts his ability to dash. A change to this location based system
	will thus probably be made just to make it look nicer and play better.


if (RootComponent->GetComponentLocation().X >= xMax - xOffset) ||
		(RootComponent->GetComponentLocation().X <= xMin + xOffset))
		return true;
		*/
	return false;

}


void ADisc::checkGoalCollision()
{
	TArray<AActor*> overlappingActors;
	GetOverlappingActors(overlappingActors);

	for (int i = 0; i < overlappingActors.Num(); ++i)
	{
		AJammingTheWindGoal* const Goal = Cast<AJammingTheWindGoal>(overlappingActors[i]);

		if (Goal)
		{
		//in the event of a goal, reports to game mode and then destroys itself.
			myGameMode = Cast<AJammingTheWindGameMode>(UGameplayStatics::GetGameMode(this));
			myGameMode->updateScore(Goal->ScoringSwitch, Goal->GoalValue);
			Destroy();
		}

	}


}

void ADisc::crossedMidlineHelper()
{
	/* If the player touches the disc twice on his own end, the opponent gains two points and the
	game is reset.
	*/
	if (StandardDirection.Y == 1 && RootComponent->GetComponentLocation().Y > midLine)
	{
		//disc made it to other side
		crossedMidline = true;
		midlineTimer = -1;

	}
		

	if (StandardDirection.Y == -1 && RootComponent->GetComponentLocation().Y < midLine)
	{
		//disc made it to other side
		crossedMidline = true;
		midlineTimer = -1;


	}

	if (playerPossessing && !crossedMidline)
	{
		//disc didn't make it to other side
		
		bool scoreSwitch;
		AJammingTheWindGameMode* myGameMode = Cast<AJammingTheWindGameMode>(UGameplayStatics::GetGameMode(this));
		if (RootComponent->GetComponentLocation().Y < midLine)
			scoreSwitch = false;
		else
			scoreSwitch = true;
		myGameMode->updateScore(scoreSwitch, 2);

	
		Destroy();
	}


}


void ADisc::checkIfInBounds()
{

	/*If due to a bug the disc gets out of bounds, we call update score without adding to either score in order
	to respawn players and flash scoreboard. The disc destroys itself.*/

	FVector check = GetActorLocation();
	if (check.X > xMax + outOfBoundsOffset ||
		check.X < xMin - outOfBoundsOffset ||
		check.Y > yMax + outOfBoundsOffset ||
		check.Y < yMin + outOfBoundsOffset)
	{

		AJammingTheWindGameMode* myGameMode = Cast<AJammingTheWindGameMode>(UGameplayStatics::GetGameMode(this));
		myGameMode->updateScore(true, 0);
		FString output;
		output = "Disc Out of Bounds, Resetting Disc";
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, *(output));
		Destroy();

	}
}

void ADisc::runDiscTimers(float &DeltaSeconds)

{
	/* We check if inbounds and then run our timers which in order assist logic with the bumper, flip event, and crossed midline event.
	*/
	checkIfInBounds();

	if (bumperCollTimer >= 0)
		bumperCollTimer += DeltaSeconds;

	if (bumperCollTimer > DeltaSeconds * 3)
		bumperCollTimer = -1;


	if (flipping)
		flipTimer += DeltaSeconds;

	if (flipping && flipTimer < 0)
		flipTimer = 0;

	if (flipTimer > flipOverTime)
	{
		flipping = false;
		flipTimer = -1;
	}

	if (flipTimer >= 0)
		flip(DeltaSeconds);

	if (!crossedMidline && midlineTimer < 0)
		midlineTimer = 0;

	if (midlineTimer >= 0)
	{
		midlineTimer += DeltaSeconds;
	}

	if (!crossedMidline && midlineTimer > midlineMinTime)
		crossedMidlineHelper();

}