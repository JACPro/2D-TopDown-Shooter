#include "GunSurvivorsGameMode.h"

AGunSurvivorsGameMode::AGunSurvivorsGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGunSurvivorsGameMode::BeginPlay()
{
	Super::BeginPlay();

	SetScore(0);
}

void AGunSurvivorsGameMode::SetScore(int NewScore)
{
	if (NewScore >= 0)
	{
		Score = NewScore;
	}
}

void AGunSurvivorsGameMode::AddScore(int AmountToAdd)
{
	int NewScore = Score + AmountToAdd;
	SetScore(NewScore);

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, FString::Printf(TEXT("Score: %d"), Score));
}
