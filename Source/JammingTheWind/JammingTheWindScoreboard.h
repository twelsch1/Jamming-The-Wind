

#pragma once

#include "GameFramework/HUD.h"
#include "JammingTheWindGameMode.h"
#include "JammingTheWindScoreboard.generated.h"

/**
 * 
 */
UCLASS()
class AJammingTheWindScoreboard : public AHUD
{
	GENERATED_UCLASS_BODY()


	//We want engine to be aware of this variable
	//and so that it will check to make sure it is
	//valid while we need it for the HUD
	//variable for storing the font;
	UPROPERTY()
	UFont* FlashingFont;

	UPROPERTY()
	UFont* ScoreboardFont;

	//Primary Draw call for the HUD
	virtual void DrawHUD() OVERRIDE;

private:
	AJammingTheWindGameMode* myGameMode;
	bool firstDraw;
	bool redLevelBool;
	float flashOffset;
	float scoreOffset;
	float redLevel;
	FVector2D leftPlayerScorePos;
	FVector2D rightPlayerScorePos;
	FVector2D leftPlayerSetsWonPos;
	FVector2D rightPlayerSetsWonPos;
	FVector2D setNumberPos;
	FVector2D leftPlayerFlashPos;
	FVector2D rightPlayerFlashPos;
	FVector2D dashPos;
	FColor myColor;
	void drawACircle();
	
};
