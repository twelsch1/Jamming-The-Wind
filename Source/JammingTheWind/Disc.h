

#pragma once

#include "GameFramework/Actor.h"
#include "JammingTheWindGameMode.h"
#include "Disc.generated.h"

/**
 * 
 */
UCLASS()
class ADisc : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Disc)
	float SpeedFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Disc)
	int32 LoserIndex;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Disc)
	FVector StandardConstant;
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Disc)
	FVector StandardDirection;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Disc)
	TSubobjectPtr<USphereComponent> BaseCollisionComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Disc)
	TSubobjectPtr<UStaticMeshComponent> DiscMesh;

	UFUNCTION(BlueprintNativeEvent)
	void SpawnMove();
	
	
	
	



	void setStandard(bool);
	void setCurved(bool);
	void setSpecial(bool);
	void setLob(bool);
	void setLobFinalLocation(FVector);
	void setXDirection(float);
	void setYDirection(float);
	void setLobDelta(FVector);
	void setMovementPerSecond(float);
	void setCurveFirstRun(bool);
	void setCrossedMidline(bool);
	void setPlayerPossessing(bool);
	void setFlipping(bool);
	void addToMovementPerSecond(float);
	void resetCurveHelper();
	void resetLobTimer();
	void calculateCurveDistance();
	float getMovementPerSecond();
	bool getFlipping();
	void orientDisc(FVector);
	virtual void Tick(float DeltaSeconds) OVERRIDE;

private:
	AJammingTheWindGameMode* myGameMode;
	bool Spawned;
	bool crossedMidline;
	bool isStandard;
	bool isCurved;
	bool isSpecial;
	bool isLobShot;
	bool aimDirLeft; //set based off player direction, initialized as false e.g aiming right for now
	bool playerPossessing;
	//bool curveRebound;
	bool curveFirstRun;
	bool flipping;
	float curveHelper;
	float curveDistance;
	float curveOffset;
	float xDirection;
	float yDirection;
	float movementPerSecond;
	float bumperCollTimer;
	float bumpCollEndTimer;
	float minYDistance;
	float bumperSpeedUp;
	float flipTimer;
	float flipOverTime;
	float zOffset;
	float xMin;
	float xMax;
	float midLine;
	float xOffset;
	float midlineTimer;
	float midlineMinTime;
	float yMin;
	float yMax;
	float outOfBoundsOffset;
	FVector lobFinalLocation;
	FVector lobDelta;
	bool bumperCollision();
	bool playerCollision();
	void checkGoalCollision();
	void curvedShot(float &);
	void specialShot(float &);
	void lobShot(float &);
	void standardShot(float &);
	void flip(float &);
	void runDiscTimers(float &);
	void crossedMidlineHelper();
	void checkIfInBounds();
	
	

	


};

FORCEINLINE void ADisc::resetCurveHelper()
{
	curveHelper = 0;
}

FORCEINLINE void ADisc::setLobFinalLocation(FVector vec)
{
	lobFinalLocation = vec;
}
FORCEINLINE void ADisc::setCrossedMidline(bool truth)
{
	crossedMidline = truth;
}


FORCEINLINE void ADisc::setLob(bool truth)
{
	isLobShot = truth;
}

FORCEINLINE void ADisc::setLobDelta(FVector vec)
{
	lobDelta = vec;
}
FORCEINLINE void ADisc::setPlayerPossessing(bool truth)
{
	playerPossessing = truth;
}

FORCEINLINE void ADisc::setCurveFirstRun(bool truth)
{
	curveFirstRun = truth;
}
FORCEINLINE void ADisc::setFlipping(bool truth)
{
	flipping = truth;
}
FORCEINLINE float ADisc::getMovementPerSecond()
{
	return movementPerSecond;
}
FORCEINLINE bool ADisc::getFlipping()
{
	return flipping;
}
FORCEINLINE void ADisc::addToMovementPerSecond(float f)
{
	movementPerSecond += f;
}

FORCEINLINE void ADisc::setMovementPerSecond(float f)
{
	movementPerSecond = f;
}

FORCEINLINE void ADisc::setXDirection(float f)
{
	xDirection = f;
}

FORCEINLINE void ADisc::setYDirection(float f)
{
	yDirection = f;
}

FORCEINLINE void ADisc::setStandard(bool truth)
{
	isStandard = truth;
}

FORCEINLINE void ADisc::setCurved(bool truth)
{
	isCurved = truth;
}

FORCEINLINE void ADisc::setSpecial(bool truth)
{
	isSpecial = truth;
}
