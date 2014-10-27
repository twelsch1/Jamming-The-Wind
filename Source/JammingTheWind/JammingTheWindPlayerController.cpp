

#include "JammingTheWind.h"
#include "JammingTheWindCharacter.h"
#include "JammingTheWindPlayerController.h"

AJammingTheWindPlayerController::AJammingTheWindPlayerController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	baseTurnRate = 90;
	smoothnessThreshold = 30;
	oneAPerFrame = true;
	oneXPerFrame = true;
	firstTick = true;
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	yMin = -2510;
	yMax = 430;
	xMin = -802.601074;
	xMax = 967.398926;
	midLine = -1040;
	xOffset = 300; 
	yOffset = 180;

	dashOverTime = 250 / 2000; //dashAmp / dashMPS in character, currently hardcoded though we could also get in the firstTick below
	playerDeltaSeconds = 0;

	controllerRotationX = 0;
	controllerRotationY = 0;
}

void AJammingTheWindPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	//we only allow one input per tick
	oneAPerFrame = true;
	oneXPerFrame = true;
	playerDeltaSeconds = DeltaTime;
	
	if (firstTick)
	{
	
		//get the character corresponding to this controller
		myChar = Cast<AJammingTheWindCharacter>(GetPawn());
		firstTick = false;
	}

}

void AJammingTheWindPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("Shot/Roll/SpecialCharge", IE_Pressed, this, &AJammingTheWindPlayerController::aButtonInput);
	InputComponent->BindAction("Flip/Lob/LobCharge", IE_Pressed, this, &AJammingTheWindPlayerController::xButtonInput);
	
	InputComponent->BindAxis("MoveForward", this, &AJammingTheWindPlayerController::moveForward);
	InputComponent->BindAxis("MoveRight", this, &AJammingTheWindPlayerController::moveRight);
	InputComponent->BindAxis("TurnRate", this, &AJammingTheWindPlayerController::turnRate);
	InputComponent->BindAxis("TurnRateY", this, &AJammingTheWindPlayerController::turnRateY);


}

void AJammingTheWindPlayerController::aButtonInput()
{
	if (oneAPerFrame)
	{
		oneAPerFrame = false;
	
		if (myChar)
			myChar->receiveAButtonInput();
	}
}

void AJammingTheWindPlayerController::xButtonInput()
{
	if (oneXPerFrame)
	{
		oneXPerFrame = false;
	
		if (myChar)
			myChar->receiveXButtonInput();

	}
}
void AJammingTheWindPlayerController::turnRate(float rate)
{

	
	float prev;
	if (myChar)
	{

		//we get how far the stick has turned from our last update and add that value

		prev = myChar->getControllerRotationX();
		myChar->setControllerRotationX(rate * baseTurnRate);

		controllerRotationX = rate * baseTurnRate;
	
		//rotationX - prev must be less than smoothness value
		//this prevents someone from hitting bottom then side 
		//without performing quarter circle motion

		if (myChar->getCurveInputEvent())
		{
			if (FMath::Abs(myChar->getControllerRotationX() - prev) < smoothnessThreshold)
				myChar->addToCurveDeltaHelper(myChar->getControllerRotationX() - prev);

		}

	}
	
}
void AJammingTheWindPlayerController::turnRateY(float rate)
{


	if (myChar)
	{
	myChar->setControllerRotationY(rate * baseTurnRate);
	controllerRotationY = rate * baseTurnRate;
	}
	

}


void AJammingTheWindPlayerController::moveForward(float value)
{
	int mult;
	if ((this != NULL) && (FMath::Abs(value) >= 0.1))
	{
		
		if (myChar)
		{

			//move forward and backwards if not in possession and movement isn't locked
			const FRotator Rotation = this->GetControlRotation();
			
			if (myChar->getYDir() == 1)
				mult = -1;

			else
				mult = 1;
			const FRotator YawRotation(0, Rotation.Yaw, 0);
		
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			

			bool willCollide = false;

			//we stop it using sphere that is larger than player component, basically that reaches out to player's arms.

			//we determine when to let player move by the direction of the STICK, not the value above....

			//if its moving away then we allow movement, otherwise we don't

			//positive going up from the bottom, negative going down from the top

			//instead of killing dash, we just make dashValue for X 0...

			if (myChar->getWallCollision())
			{
				if (myChar->GetActorLocation().X < 0) //lower wall
				{
					if (controllerRotationY < 0)
					{
						willCollide = true;
						myChar->resetXDashing();
					}
				}
				
				else //top wall
				{
					if (controllerRotationY > 0)
					{
						willCollide = true;
						myChar->resetXDashing();
					}
				}
			}
			if (!myChar->getInPossession() && !myChar->getMovementLocked() && !willCollide)
			{

				myChar->AddMovementInput(Direction, value);
			//	FString output = FString::SanitizeFloat(value);
			//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *(output));

			}
		}
	}
}

void AJammingTheWindPlayerController::moveRight(float value)
{
	int mult;
	if ((this != NULL) && (FMath::Abs(value) >= 0.1))
	{
		
		
		if (myChar)
		{
			//move right and left if not in possession and movement isn't locked

			const FRotator Rotation = this->GetControlRotation();

				if (myChar->getYDir() == 1)
						mult = 1;

				else
					mult = -1;
			const FRotator YawRotation(0, Rotation.Yaw * -1, 0);

			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

			
			bool willCollide = false;
			//if on the right rotationX has to be less than 0, if on the left greater than 0
			//ydir is -1 on the right
			if (myChar->getGoalCollision())
			{

				if (myChar->getYDir() == -1)
				{

					if (controllerRotationX > 0)
					{
						willCollide = true;
						myChar->resetYDashing();
					}

				}

				else
				{
					if (controllerRotationX < 0)
					{
						willCollide = true;
						myChar->resetYDashing();

					}

				}
			}

				if (!myChar->getInPossession() && !myChar->getMovementLocked() && !willCollide)
				{

					myChar->AddMovementInput(Direction, value);
				//	FString output = FString::SanitizeFloat(value);
				//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *(output));
				}
			}
		}
	
}



