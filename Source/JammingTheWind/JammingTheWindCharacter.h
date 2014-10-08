// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "Disc.h"
#include "JammingTheWindCharacter.generated.h"

UCLASS(Blueprintable)
class AJammingTheWindCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

	/** Top down camera */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TSubobjectPtr<UCameraComponent> TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TSubobjectPtr<class USpringArmComponent> CameraBoom; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyChar)
	bool ReceiveDisc;


	virtual void Tick(float DeltaSeconds) OVERRIDE;

	void receiveAButtonInput();
	void receiveXButtonInput();
	void setControllerRotationX(float);
	void setControllerRotationY(float);
	void setMovementLocked(bool);
	void resetHelper();
	void setYDir(int);
	void addToCurveDeltaHelper(float);
	//bool getStrafing();
	bool getCurveInputEvent();
	bool getDashing();
	bool getMovementLocked();
	bool chargeAButton();
	bool chargeXButton();
	bool flipOrCharge();
	float getControllerRotationX();
	int getYDir();
	int getXDir();
	bool getInPossession();

private:
	bool inPossession;
	ADisc* disc;
	FVector discSetLocation;
	float controllerRotationX;
	float controllerRotationY;
	float curveTimer;
	float curveDeltaHelper;
	float curveDelta;
	float curveInputTime;
	float curveOutputTime;
	int xDir;
	int yDir;
	int index;
	float speedUp;
	float slowDown;
	float minSpeed;
	float maxSpeed;
	float dashAmplitude;
	float shotAmplitude;
	float dashMovementPerSecond;
	float dashOverTime;
	float dashingTimer;
	float rotationAdjustment;
	float rotationPerSecond;
	float curveLeeway;
	float rotationLeeway;
	float xDashDirection;
	float yDashDirection;
	float orientDiscTimer;
	float orientDiscOverTime;
	float zOffset;
	float yOffset;
	float discCollTimer;
	float discCollEndTimer;

	float refreshTimer;
	float refreshEndTimer;

	float flipTimer;
	float flipEndTimer;
	float flipDontAllowNewInput;
	bool flipEvent;


	float chargeMinTime;
	int chargeMinMashes;
	int chargeXMashes;
	int chargeAMashes;
	bool xCharged;
	bool aCharged;

	float lobMidlineOffset;
	float lobBacklineOffset;
	float midline;

	float xMin;
	float xMax;
	float xOffset;
	float yMin;
	float yMax;
	

	FVector orientLocationHelper;

	int updates;

	bool startOfPossession; 
	bool orientDisc;
	bool curveInputEvent;
	bool curveEvent;
	//bool strafing;
	bool startRot;
	//bool startPoss;
	bool dashing; 
	bool discDashing;
	bool hasShot;
	bool shotFree;
	bool movementLocked;

	TSubobjectPtr<class USphereComponent> dashSphere;


	float calculateXDirection(float &);
	float calculateYDirection(float &);
	bool isNotMoving();
	bool isFacingForward();
	bool isDiscColliding();
	bool isCurveTopInput();
	bool isCurveBottomInput();
	bool discSet();
	bool getStickInput();
	void calculateRotationAdjustment();
	FVector calculateLobDelta();
	FVector calculateDiscDelta(float &);
	void animateToFront(float &);
	void dash(float &);
	void dashCollision();
	void runTimers(float &);
	void flipEventHelper(float &);
	void curveEventHelper(float &);
	void chargeShotHelper();




};

FORCEINLINE void AJammingTheWindCharacter::resetHelper()
{
	startOfPossession = false;
	inPossession = false;
	discDashing = false;
	dashing = false;
	hasShot = true;
	shotFree = true;
}
FORCEINLINE bool AJammingTheWindCharacter::isCurveTopInput()
{
	return (controllerRotationX <= curveLeeway && controllerRotationX >= -curveLeeway) && (controllerRotationY <= -90 + curveLeeway && controllerRotationY >= -90 - curveLeeway);
}

FORCEINLINE bool AJammingTheWindCharacter::isCurveBottomInput()
{
	return (controllerRotationX <= curveLeeway && controllerRotationX >= -curveLeeway) && (controllerRotationY <= 90 + curveLeeway && controllerRotationY >= 90 - curveLeeway);
}

FORCEINLINE bool AJammingTheWindCharacter::isNotMoving()
{
	return RootComponent->GetComponentVelocity().X == 0 && RootComponent->GetComponentVelocity().Y == 0 && !dashing;
}
FORCEINLINE bool AJammingTheWindCharacter::getStickInput()
{
	return RootComponent->GetComponentVelocity().X != 0 || RootComponent->GetComponentVelocity().Y != 0; 
}
FORCEINLINE bool AJammingTheWindCharacter::isFacingForward()
{
	return RootComponent->GetComponentRotation().Yaw <= (yDir * 90) + rotationLeeway && RootComponent->GetComponentRotation().Yaw >= (yDir * 90) - rotationLeeway;
}
FORCEINLINE void AJammingTheWindCharacter::setControllerRotationX(float xRot)
{
	controllerRotationX = xRot;
}

FORCEINLINE void AJammingTheWindCharacter::setYDir(int dir)
{
	yDir = dir;
}

FORCEINLINE void AJammingTheWindCharacter::setControllerRotationY(float yRot)
{
	controllerRotationY = yRot;
}

FORCEINLINE void AJammingTheWindCharacter::setMovementLocked(bool truth)
{
	movementLocked = truth;
}
FORCEINLINE void AJammingTheWindCharacter::addToCurveDeltaHelper(float additive)
{
	curveDeltaHelper += additive;
}


FORCEINLINE bool AJammingTheWindCharacter::getDashing()
{
	return dashing;
}

FORCEINLINE bool AJammingTheWindCharacter::getCurveInputEvent()
{
	return curveInputEvent;
}

FORCEINLINE bool AJammingTheWindCharacter::getInPossession()
{

	return inPossession;
}

FORCEINLINE bool AJammingTheWindCharacter::getMovementLocked()
{
	return movementLocked;
}

FORCEINLINE int AJammingTheWindCharacter::getYDir()
{
	return yDir;
}

FORCEINLINE int AJammingTheWindCharacter::getXDir()
{
	return xDir;
}

FORCEINLINE float AJammingTheWindCharacter::getControllerRotationX()
{
	return controllerRotationX;
}



FORCEINLINE bool AJammingTheWindCharacter::flipOrCharge()
{
	
	return xCharged || aCharged || flipEvent;
}


