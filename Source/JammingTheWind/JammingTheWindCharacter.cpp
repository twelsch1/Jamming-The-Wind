// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "JammingTheWind.h"
#include "Disc.h"
#include "JammingTheWindGoal.h"
#include "JammingTheWindPlayerController.h"
#include "JammingTheWindBumper.h"
#include "JammingTheWindGameMode.h"
#include "JammingTheWindCharacter.h"

AJammingTheWindCharacter::AJammingTheWindCharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// Set size for player capsule
	CapsuleComponent->InitCapsuleSize(42.f, 96.f); //42 and 96 were default, then we did x2 for some reason
	//CapsuleComponent->SetHiddenInGame(false);



	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	CharacterMovement->bOrientRotationToMovement = true; // Rotate character to moving direction, true if not strafing
	CharacterMovement->RotationRate = FRotator(0.f, 640.f, 0.f);
	CharacterMovement->bConstrainToPlane = true;
	CharacterMovement->bSnapToPlaneAtStart = true;
	startRot = true;
	rotationAdjustment = 0;
	inPossession = false;
	hasShot = true;
	movementLocked = false;
	controllerRotationX = 0;
	controllerRotationY = 0;
	curveTimer = 0;
	curveDeltaHelper = 0;
	
	curveInputEvent = false;
	curveEvent = false;
	
	xDir = 1;
	yDir = 1;
	index = 0;

	shotFree = true;

	//determines the final resting place of disc in relation to character 
	//and how it animates
	zOffset = 240;
	yOffset = 120;
	orientDiscTimer = -1;
	orientDiscOverTime = 0.1;
	orientDisc = false;
	startOfPossession = false;
	orientLocationHelper = { 0, 0, 0 };
	discSetLocation = { 0, 0, 0 };
	
	//strafing = false;

	//determine direction time and duration of dash
	dashing = false;
	yDashDirection = 0;
	xDashDirection = 0;
	dashingTimer = -1;
	dashMovementPerSecond = 2000;
	dashAmplitude = 250; //was 500
	dashOverTime = dashAmplitude / dashMovementPerSecond;

	refreshTimer = -1;
	refreshEndTimer = 0.3;


	discCollTimer = -1;
	discCollEndTimer = 0.1;

	//flipping variables
	flipTimer = -1;
	flipEndTimer = 0.4; //we had at 0.1, since it is more of a block we currently set as just under half a second
	flipDontAllowNewInput = 0.8; //we increased the punishment time from 0.5
	flipEvent = false;

	//charging variables
	chargeMinTime = 1;
	chargeXMashes = 0;
	chargeAMashes = 0;
	chargeMinMashes = 7;
	xCharged = false;
	aCharged = false;

	midline = -1040;
	lobMidlineOffset = 250;
	lobBacklineOffset = 1020;
	xMin = -802.601074;
	xMax = 967.398926;
	xOffset = 338; //200 from edge
	yMin = -2510;
	yMax = 430;

	//modify below to gameplay feel
	speedUp = 200; //amount disc speeds up when shot
	slowDown = -250; //amount disc slows down as its being held per second
	minSpeed = 2000; //the minimum speed the disc can reach
	maxSpeed = 3500; //the maximum speed the disc can reach
	curveLeeway = 8; //the margin of error we allow for starting curve event 
	rotationPerSecond = 180 * 4; //e.g the max 180 rotation would take 1/4 of a second
	rotationLeeway = 0.1; //leeway in degrees for orientation event
	shotAmplitude = 4000; //used in trig functions below
	curveInputTime = 0.3; //the amount of time player has to do quarter circle
	curveOutputTime = 0.2; //amount of time player has after successful quarter circle to press a
	curveDelta = 65; //the amount of degrees the stick has to move to register quarter circle
	//note that the ideal player would go 90, but this is meant to give player a large margin of error
	//and do what they meant rather than what they executed

	

	// Create a camera boom...
	CameraBoom = PCIP.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	FVector vec = { 60.79541, -1051.10, 1999.262 };
	FRotator rot = { -90, 0, 0 };
	CameraBoom->SetRelativeLocationAndRotation(vec, rot);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 400; //was -100
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("TopDownCamera"));
	TopDownCameraComponent->AttachTo(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUseControllerViewRotation = false; // Camera does not rotate relative to arm

	
}


void AJammingTheWindCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	// run our support timers
	runTimers(DeltaSeconds);

/*
Debugging output, used for video series
FString output = "yDir: " + FString::SanitizeFloat(calculateXDirection(shotAmplitude));
GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *(output));
output = "xDir: " + FString::SanitizeFloat(calculateYDirection(shotAmplitude));
GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, *(output));  
Note that y output and x output are reversed. The actual x/y axes of the world are
flipped from what would normally be expected e.g from the player's perspective, the horizontal
plane is actually y and vertical x.*/

	if (dashing)
	{
		dash(DeltaSeconds);
	}

	//some logic to help animate character to the front when they aren't moving
	if (isNotMoving())
	{
		if (startRot)
		{
			calculateRotationAdjustment();
			startRot = false;
		}
		
		if (isFacingForward())
			startRot = true;
		
		else
			animateToFront(DeltaSeconds);
	}
	

	if (!inPossession)
		startOfPossession = true;
	
	
	
	//the player is possessing until the disc has been shot, so it only checks when hasShot is true and disc isn't flipping
	if (hasShot && discCollTimer < 0 && !flipOrCharge())
		inPossession = isDiscColliding();
	
	flipEventHelper(DeltaSeconds);
	//if player catches it on dash, we move the disc to the offset.
	if (inPossession && dashing)
	{
		orientLocationHelper = RootComponent->GetComponentLocation();
		orientLocationHelper.Set(orientLocationHelper.X, orientLocationHelper.Y, zOffset);
		discSetLocation = orientLocationHelper + FVector(0, yOffset * yDir, 0);
		disc->SetActorLocationAndRotation(discSetLocation, FRotator(0, 0, 0));
	} 

	if (inPossession && !dashing)
	{		
		if (startOfPossession)
		{
			if (flipEvent)
			{
				disc->setFlipping(true);
			}

		//if flipEvent, the disc starts flipping
		//we reset some variables and move the disc to the offet
		hasShot = false;
		chargeXMashes = 0;
		chargeAMashes = 0;
	
		orientLocationHelper = RootComponent->GetComponentLocation();
		orientLocationHelper.Set(orientLocationHelper.X, orientLocationHelper.Y, zOffset);
		discSetLocation = orientLocationHelper + FVector(0, yOffset * yDir, 0);
		disc->SetActorLocationAndRotation(discSetLocation, FRotator(0, 0, 0));
		disc->resetCurveHelper();
		startOfPossession = false;



		}
		
		//in first tick and every other tick we slow down disc until
		//launched and we check for curve shot or charge shot
	
		disc->addToMovementPerSecond(slowDown * DeltaSeconds);
	
		if (disc->getMovementPerSecond() < minSpeed)
			disc->setMovementPerSecond(minSpeed);
		
	
		curveEventHelper(DeltaSeconds);

		chargeShotHelper();
		

	
	}

}
void AJammingTheWindCharacter::receiveXButtonInput()
{
	//e.g can't flip once already in possession, can't do it while mid dash, and can't do it on the first throw 

	if (!inPossession && !dashing && flipTimer < 0 && !movementLocked)
	{

		flipTimer = 0;

	}
	
	if (inPossession && flipEvent) //just for the buttonMashing
	{

		chargeXMashes++;
		
	}
	if (inPossession && !flipOrCharge() && !dashing && !startOfPossession) //lobshot 
	{
		
		FVector delta = calculateLobDelta();
	
		disc->setLobDelta(delta);
		AJammingTheWindGameMode* myGameMode = Cast<AJammingTheWindGameMode>(UGameplayStatics::GetGameMode(this));
		delta += disc->GetActorLocation();
		
		disc->setLobFinalLocation(delta);

		disc->StandardDirection.Set(xDir, yDir, disc->StandardDirection.Z);
		myGameMode->setLobDiscLocation(delta);
		myGameMode->setFlashLobDisc(true);
		//we temporarily lock movement so player's aim does not translate into movement in next frame
		//inform disc player is no longer in possession and to check for when it crosses the midline

		movementLocked = true;
		refreshTimer = 0;
		disc->setPlayerPossessing(false);
		disc->setCrossedMidline(false);
		//reset variables for character
		inPossession = false;
		hasShot = true;
		discCollTimer = 0;
		curveInputEvent = false;
		curveEvent = false;
		curveTimer = 0;

		//set lob shot to true and all others to false
		disc->setSpecial(false);
		disc->setStandard(false);
		disc->setCurved(false);
		disc->setLob(true);
		
		
		
		
	}
	


	
}
void AJammingTheWindCharacter::receiveAButtonInput()
{
	if (inPossession && !flipOrCharge() && !dashing && !startOfPossession) 
	{

		disc->addToMovementPerSecond(speedUp);

		if (disc->getMovementPerSecond() > maxSpeed)
			disc->setMovementPerSecond(maxSpeed);
		disc->StandardDirection.Set(xDir, yDir, disc->StandardDirection.Z);
		disc->setPlayerPossessing(false);
		disc->setCrossedMidline(false);

		if (curveEvent) //curve shot
		{
		
			disc->setCurveFirstRun(true);
			movementLocked = true;
			refreshTimer = 0;
			disc->setSpecial(false);
			disc->setStandard(false);
			disc->setCurved(true);
			disc->setLob(false);
			hasShot = true;
			discCollTimer = 0;
			curveInputEvent = false;
			curveEvent = false;
			curveTimer = 0;
			inPossession = false;

		}
		else
		{
			//standard shot
		
			disc->setXDirection(calculateXDirection(shotAmplitude));
			disc->setYDirection(calculateYDirection(shotAmplitude));
	
			movementLocked = true;
			refreshTimer = 0;
			disc->setSpecial(false);
			disc->setStandard(true);
			disc->setCurved(false);
			disc->setLob(false);
		
		
			hasShot = true;
			discCollTimer = 0;
			curveInputEvent = false;
			curveEvent = false;
			curveTimer = 0;
			inPossession = false;
		}
		
	}
	if (inPossession && flipEvent) //button mash charge input
	{
		chargeAMashes++;

	}

	if (!inPossession && !dashing && !movementLocked && getStickInput()) //when not in possession and stick is moving and we aren't movement locked
	{
		//begin dashing
		dashing = true;
		dashingTimer = 0;
		xDashDirection = calculateXDirection(dashAmplitude);
		yDashDirection = calculateYDirection(dashAmplitude);
	
	}
		
}
float AJammingTheWindCharacter::calculateXDirection(float& amplitude)
{
	//again note that we use Sin for X as the axes our flipped from player's perspective in game
	//we convert to degrees then multiply the trig statement by amplitude
	return FMath::Sin((controllerRotationY) / 180 * PI) * amplitude;
	
}
float AJammingTheWindCharacter::calculateYDirection(float& amplitude)
{

	return FMath::Cos((controllerRotationY) / 180 * PI) * amplitude;

}
void AJammingTheWindCharacter::animateToFront(float& DeltaSeconds)
{
	/* Using rotation adjustment, we rotate character so that he faces toward the opposite goal. (e.g 90 or -90 degrees)
	*/
	float overTime;
	FRotator Delta;
	int updates = 100;
	
	overTime = FMath::Abs(rotationAdjustment)/rotationPerSecond;
	orientDiscOverTime = overTime;
	
	Delta = { 0, rotationAdjustment / overTime * DeltaSeconds / updates, 0 };

	for (int i = 0; i < updates; ++i)
	{
		if (RootComponent->GetComponentRotation().Yaw <= (yDir * 90) + rotationLeeway && RootComponent->GetComponentRotation().Yaw >= (yDir * 90) - rotationLeeway)
			i += updates;
		else
			RootComponent->AddRelativeRotation(Delta);

	}
}
void AJammingTheWindCharacter::calculateRotationAdjustment()
{
	//calculate the shortest rotation to orient towards the front
	float currentRotation, targetRotation, dirOne, dirTwo, mult;
	currentRotation = RootComponent->GetComponentRotation().Yaw;
	targetRotation = yDir * 90;
	dirOne = targetRotation - currentRotation; 
	//figure out the other direction
	if (dirOne < 0)
		mult = 1;
	else
		mult = -1;
	dirTwo = mult * (360 - FMath::Abs(dirOne)); //this calculates the complement of dirOne in the opposite direction

	if (FMath::Abs(dirOne) < FMath::Abs(dirTwo))
		rotationAdjustment = dirOne;
	else
		rotationAdjustment = dirTwo;

	
	
}
void AJammingTheWindCharacter::dash(float &DeltaSeconds)
{
	int mult, updates;
	updates = 1;
	FVector Delta;
	if (RootComponent->GetComponentRotation().Yaw < 0)
		mult = -1;
	else
		mult = 1;

	//we move the character an extra amount in the direction they are already aiming to create a dash effect
	for (int i = 0; i < updates; ++i)
	{
		dashCollision();
		
		Delta.Set(xDashDirection / dashOverTime * (DeltaSeconds / updates), mult * yDashDirection / dashOverTime * (DeltaSeconds / updates), 0);
		if (dashing)
			RootComponent->MoveComponent(Delta, RootComponent->GetComponentRotation(), true);
		
		else
			i += updates;
	

		dashCollision();

	}
}
void AJammingTheWindCharacter::dashCollision()
{
	//check to see if player is running into a wall or a goal
	TArray<AActor*> overlappingActors;
	
	GetOverlappingActors(overlappingActors);
	for (int i = 0; i < overlappingActors.Num(); ++i)
	{
		AJammingTheWindBumper* const myBumper = Cast<AJammingTheWindBumper>(overlappingActors[i]);

		AJammingTheWindGoal* const myGoal = Cast<AJammingTheWindGoal>(overlappingActors[i]);


		if (myGoal)
		{
			//e.g if it's not the floor
			if (myGoal->GoalValue != 2)
			{

				dashing = false;
				dashingTimer = -1;
			}

		}

		if (myBumper)
		{
			dashing = false;
			dashingTimer = -1;

		}
		

	}
	
	

}
bool AJammingTheWindCharacter::isDiscColliding()
{
	//when disc collides, we set disc to myDisc and return true to start possession
	TArray<AActor*> overlappingActors;
	GetOverlappingActors(overlappingActors);
	for (int i = 0; i < overlappingActors.Num(); ++i)
	{
		ADisc* const myDisc = Cast<ADisc>(overlappingActors[i]);

		AJammingTheWindGoal* const myGoal = Cast<AJammingTheWindGoal>(overlappingActors[i]);

		if (myDisc)
		{
			disc = myDisc;
			disc->setPlayerPossessing(true);
			return true;

		}

	}
	return false;

}

FVector AJammingTheWindCharacter::calculateDiscDelta(float &DeltaSeconds)
{

	//deprecated code
	

	FVector finalDiscDest, discDelta;

	discDelta = { 0, yOffset * yDir, 0 };
	finalDiscDest = orientLocationHelper + discDelta;
	finalDiscDest.Set(finalDiscDest.X, finalDiscDest.Y, zOffset);
	discDelta = finalDiscDest - disc->GetActorLocation();

	discDelta = { discDelta.X / orientDiscOverTime * DeltaSeconds , discDelta.Y / orientDiscOverTime * DeltaSeconds , 0};

	return discDelta;

}
void AJammingTheWindCharacter::runTimers(float &DeltaSeconds)
{
	if (flipEvent)
	{

		if (chargeAButton())
		{
			aCharged = true;
			flipEvent = false;

		}
		if (chargeXButton() && flipEvent) //e.g in case chargeAButton already happening
		{
			xCharged = true;
			flipEvent = false;

		}
	}
	if (refreshTimer >= 0)
		refreshTimer += DeltaSeconds;
	if (refreshTimer > refreshEndTimer)
	{
		movementLocked = false;
		refreshTimer = -1;
	}
	if (orientDiscTimer >= 0)
		orientDiscTimer += DeltaSeconds;

	if (orientDiscTimer > orientDiscOverTime)
	{
		orientDisc = false;
		orientDiscTimer = -1;
	}

	if (dashingTimer >= 0)
		dashingTimer += DeltaSeconds;

	if (dashingTimer > dashOverTime)
	{
		dashingTimer = -1;
		dashing = false;

	}

	if (discCollTimer >= 0)
		discCollTimer += DeltaSeconds;

	if (discCollTimer > discCollEndTimer)
		discCollTimer = -1;
}

void AJammingTheWindCharacter::flipEventHelper(float &DeltaSeconds)
{
	if (flipTimer >= 0)
		flipTimer += DeltaSeconds;
	if (flipTimer > 0 && flipTimer <= flipEndTimer && inPossession) //as long as player possesses during this short window, flipEvent becomes true
	{
		flipEvent = true;
		flipTimer = -1;
	}
	if (flipTimer > flipEndTimer + flipDontAllowNewInput)
		flipTimer = -1;
}
void AJammingTheWindCharacter::curveEventHelper(float &DeltaSeconds)
{
	//if player moves stick to correct position, we check for quarter circle
	if (isCurveTopInput())
	{
		curveInputEvent = true;
		xDir = 1;
		

	}

	else if (isCurveBottomInput())
	{
		curveInputEvent = true;
		xDir = -1;

	}

	//check for quarter circle
	if (curveInputEvent)
	{
		curveTimer += DeltaSeconds;
		if (curveTimer > curveInputTime)
		{
			curveTimer = 0;
			curveInputEvent = false;
			curveDeltaHelper = 0;
		}
		if (curveDeltaHelper * yDir >= curveDelta)
		{
			curveTimer = 0;
			curveEvent = true;
			curveDeltaHelper = 0;
			curveInputEvent = false;

		}
	

	}
	//if quarter circle, we set curveEvent to true
	//if input a is made during curve event, we launch curve shot
	if (curveEvent)
	{
		curveTimer += DeltaSeconds;
		if (curveTimer > curveOutputTime + DeltaSeconds)
		{
			
			curveEvent = false;
			curveTimer = 0;
		}
	}

}

void AJammingTheWindCharacter::chargeShotHelper()
{
	if (flipOrCharge() && !disc->getFlipping()) //e.g disc done with flipEvent
	{

		if (flipEvent)
		{
			//end flip event, don't allow for curve event to start in the air
			curveInputEvent = false;
			curveEvent = false;
			curveTimer = 0;
			flipEvent = false;
		
		}
	
		if (aCharged || xCharged)
		{
			//special shot...
			disc->SetActorLocationAndRotation(FVector(disc->GetActorLocation().X, disc->GetActorLocation().Y, zOffset), FRotator(0, 0, 0));
			if (controllerRotationY < 0)
				disc->StandardDirection.Set(-1, yDir, disc->StandardDirection.Z);
			else
				disc->StandardDirection.Set(1, yDir, disc->StandardDirection.Z);
			disc->setSpecial(true);
			disc->setStandard(false);
			disc->setCurved(false);
			disc->setLob(false);

			refreshTimer = 0;
			movementLocked = true;
			hasShot = true;
			discCollTimer = 0;
			curveInputEvent = false;
			curveEvent = false;
			curveTimer = 0;
			aCharged = false;
			xCharged = false;
			inPossession = false;
			disc->setPlayerPossessing(false);
		

		}


	}
}
bool AJammingTheWindCharacter::chargeAButton()
{
	AJammingTheWindPlayerController* myController = Cast<AJammingTheWindPlayerController>(GetController());
	return myController->getAButtonHeld() >= chargeMinTime || chargeAMashes >= chargeMinMashes;
}

bool AJammingTheWindCharacter::chargeXButton()
{
	AJammingTheWindPlayerController* myController = Cast<AJammingTheWindPlayerController>(GetController());
	return myController->getXButtonHeld() >= chargeMinTime || chargeXMashes >= chargeMinMashes;
}

FVector AJammingTheWindCharacter::calculateLobDelta()
{
	float yDistance, xDistance, speedAdditive, lobXMax, lobXMin, lobYMin;
	lobYMin = 2940 * 0.4;
	speedAdditive = (lobBacklineOffset / (maxSpeed - minSpeed) * (disc->getMovementPerSecond() - minSpeed));

	//first get the distance to midline + offset THEN add the speedAdditive
	if (yDir == 1)
	{
		yDistance = midline + lobMidlineOffset - disc->GetActorLocation().Y + speedAdditive;
		if (yDistance < lobYMin)
			yDistance = lobYMin;
	}
	if (yDir == -1)
	{

		yDistance = midline - lobMidlineOffset - disc->GetActorLocation().Y - speedAdditive;
		if (yDistance > -1 * lobYMin)
			yDistance = -1 * lobYMin;
	}
	//get ratio of x to y and then multiply by y Distance
	xDistance = (calculateXDirection(shotAmplitude) /calculateYDirection(shotAmplitude)) * FMath::Abs(yDistance);

	
	lobXMax = xMax - xOffset;
	lobXMin = xMin + xOffset;
	//if negative
	if (xDistance < 0)
	{
		if (disc->GetActorLocation().X + xDistance < lobXMin)
			xDistance = lobXMin - disc->GetActorLocation().X;
	}
	//if positive
	else
	{
		if (disc->GetActorLocation().X + xDistance > lobXMax)
			xDistance = lobXMax - disc->GetActorLocation().X;
	}
	
	FVector Delta(xDistance, yDistance, 0);

	return Delta;
} 

bool AJammingTheWindCharacter::discSet()
{
	//deprecated
	if (disc->GetActorLocation() == discSetLocation)
		return true;
	else
		return false;
}
