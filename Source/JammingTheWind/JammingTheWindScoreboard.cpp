

#include "JammingTheWind.h"
#include "JammingTheWindGameMode.h"
#include "JammingTheWindScoreboard.h"


AJammingTheWindScoreboard::AJammingTheWindScoreboard(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

	static ConstructorHelpers::FObjectFinder<UFont>HUDFontOb(TEXT("/Game/Materials/flashFont"));
	static ConstructorHelpers::FObjectFinder<UFont>HUDFontObTwo(TEXT("/Engine/EngineFonts/RobotoDistanceField"));
	FlashingFont = HUDFontOb.Object;
	ScoreboardFont = HUDFontObTwo.Object;
	flashOffset = 12/900;
	scoreOffset = 6/900;
	redLevel = 0;
	redLevelBool = true;
	firstDraw = true;

}


void AJammingTheWindScoreboard::DrawHUD()
{
	
	

	


	//call to the parent versions of DrawHud
	Super::DrawHUD();

	if (firstDraw)
	{
		//get the game mode, then establish our positions for posting score
		//we use the fractions and base it off of canvas size so that it will work on
		//screens of different sizes.
		myGameMode = Cast<AJammingTheWindGameMode>(UGameplayStatics::GetGameMode(this));
		leftPlayerScorePos = FVector2D(Canvas->SizeX * 0.4 - Canvas->SizeX * scoreOffset, 0.05 * Canvas->SizeY);
		rightPlayerScorePos = FVector2D(Canvas->SizeX * 0.6 - Canvas->SizeX * scoreOffset, 0.05 * Canvas->SizeY);
		setNumberPos = FVector2D(Canvas->SizeX * 0.5 - Canvas->SizeX * scoreOffset, 0.09 * Canvas->SizeY);
		leftPlayerSetsWonPos = FVector2D(Canvas->SizeX * 0.15 - Canvas->SizeX * scoreOffset, 0.09 * Canvas->SizeY);
		rightPlayerSetsWonPos = FVector2D(Canvas->SizeX * 0.85 - Canvas->SizeX * scoreOffset, 0.09 * Canvas->SizeY);

		leftPlayerFlashPos = FVector2D(Canvas->SizeX * 0.35 - Canvas->SizeX * flashOffset, 0.4 * Canvas->SizeY);
		rightPlayerFlashPos = FVector2D(Canvas->SizeX * 0.65 - Canvas->SizeX * flashOffset, 0.4 * Canvas->SizeY);
		dashPos = FVector2D(Canvas->SizeX * 0.5 - Canvas->SizeX * flashOffset, 0.4 * Canvas->SizeY);
		firstDraw = false;
	}
	
	//Post Score information
	DrawText(FString::FromInt(myGameMode->getSetNumber()), FColor::White, setNumberPos.X, setNumberPos.Y, ScoreboardFont);
	DrawText(FString::FromInt(myGameMode->getLeftPlayerScore()), FColor::Blue, leftPlayerScorePos.X, leftPlayerScorePos.Y, ScoreboardFont);
	DrawText(FString::FromInt(myGameMode->getRightPlayerScore()), FColor::Red, rightPlayerScorePos.X, rightPlayerScorePos.Y, ScoreboardFont);
	DrawText(FString::FromInt(myGameMode->getLeftPlayerSetsWon()), FColor::Blue, leftPlayerSetsWonPos.X, leftPlayerSetsWonPos.Y, ScoreboardFont);
	DrawText(FString::FromInt(myGameMode->getRightPlayerSetsWon()), FColor::Red, rightPlayerSetsWonPos.X, rightPlayerSetsWonPos.Y, ScoreboardFont);

	//In the event of a lob shot, we draw the location where it will land
	if (myGameMode->getFlashLobDisc())
	{
		//we alternate taking red level up and down to cause the circle to flash
		if (redLevelBool)
		{
			redLevel += 15;

			if (redLevel > 200)
				redLevelBool = false;

		}

		else
		{
			redLevel -= 15;
			if (redLevel <= 0)
			{
				redLevel = 0;
				redLevelBool = true;
			}
		}
		drawACircle();
	}

	else
	{
		redLevel = 0;
		redLevelBool = true;
	}
	
	//On score, flash a much larger version of current score in center of screen
	if (myGameMode->getFlashScoreboard())
	{
			DrawText(FString::FromInt(myGameMode->getLeftPlayerScore()), FColor::Blue, leftPlayerFlashPos.X, leftPlayerFlashPos.Y, FlashingFont);
			DrawText(FString::FromInt(myGameMode->getRightPlayerScore()), FColor::Red, rightPlayerFlashPos.X, rightPlayerFlashPos.Y, FlashingFont);
			DrawText("-", FColor::Black, dashPos.X, dashPos.Y, FlashingFont);
		
	} 


}
void AJammingTheWindScoreboard::drawACircle()
{
	//draws a circle, with the color being based off redLevel so as to make it flash
	const int numSides = 16; //number of lines to be drawn
	const float radius = 7;
	const float radiansDelta = 2 * PI / numSides; //angle can be visualized like a slice of pie

	FVector X(4, 0, 0);
	FVector Y(0, 4, 0);
	FVector Base = Canvas->Project(myGameMode->getLobDiscLocation());
	FVector LastVertex = Base + X * radius;

	for (int i = 0; i < numSides; ++i)
	{
		//basic use of trigonometry to cause the lines to be drawn in a circle
		//we do i + 1 due to the index starting at 0
		const FVector Vertex = Base + (X * FMath::Cos(radiansDelta * (i + 1)) + Y * FMath::Sin(radiansDelta * (i + 1))) * radius;
		DrawDebugCanvas2DLine(Canvas, LastVertex, Vertex, FColor::FColor(255, redLevel, 0, 0));
		LastVertex = Vertex;

	}
}

