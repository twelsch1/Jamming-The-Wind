

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
	
}

void AJammingTheWindPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	//we only allow one input per tick
	oneAPerFrame = true;
	oneXPerFrame = true;
	
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
	}
	

}


void AJammingTheWindPlayerController::moveForward(float value)
{
	if ((this != NULL) && (value != 0.0f))
	{
		
		if (myChar)
		{

			//move forward and backwards if not in possession and movement isn't locked
			const FRotator Rotation = this->GetControlRotation();

			const FRotator YawRotation(0, Rotation.Yaw, 0);
		
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			if (!myChar->getInPossession() && !myChar->getMovementLocked())
			{

				myChar->AddMovementInput(Direction, value);
			}
		}
	}
}

void AJammingTheWindPlayerController::moveRight(float value)
{
	if ((this != NULL) && (value != 0.0f))
	{
		
		
		if (myChar)
		{
			//move right and left if not in possession and movement isn't locked

			const FRotator Rotation = this->GetControlRotation();
			
			const FRotator YawRotation(0, Rotation.Yaw * -1, 0);

			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			
			if (!myChar->getInPossession() && !myChar->getMovementLocked())
			{

				myChar->AddMovementInput(Direction, value);

			}
		}
	
	}
}


