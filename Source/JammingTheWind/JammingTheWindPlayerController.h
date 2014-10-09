
#pragma once

#include "JammingTheWindCharacter.h"
#include "JammingTheWindPlayerController.generated.h"

UCLASS()
class AJammingTheWindPlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()
	
	float getAButtonHeld();
	float getXButtonHeld();

private:

	virtual void PlayerTick(float DeltaTime) OVERRIDE;
	virtual void SetupInputComponent() OVERRIDE;

	void moveForward(float);
	void moveRight(float);
	void turnRate(float);
	void turnRateY(float);
	void aButtonInput();
	void xButtonInput();

	AJammingTheWindCharacter* myChar;
	bool oneAPerFrame;
	bool oneXPerFrame;
	bool firstTick;
	float baseTurnRate;
	float smoothnessThreshold;


};


FORCEINLINE float AJammingTheWindPlayerController::getAButtonHeld()
{
	FKey f;
	f = EKeys::Gamepad_FaceButton_Bottom;
	return GetInputKeyTimeDown(f);
}

FORCEINLINE float AJammingTheWindPlayerController::getXButtonHeld()
{
	FKey f;
	f = EKeys::Gamepad_FaceButton_Left;
	return GetInputKeyTimeDown(f);
}