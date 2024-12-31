#include "EnemySpawner.h"

#include "Kismet/GameplayStatics.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	AGameModeBase* BaseGameMode = UGameplayStatics::GetGameMode(GetWorld());
	if (BaseGameMode)
	{
		GameMode = Cast<AGunSurvivorsGameMode>(BaseGameMode);
		check(GameMode);
	}
	
	AActor* PlayerActor = UGameplayStatics::GetActorOfClass(GetWorld(), ATopDownCharacter::StaticClass());
	if (PlayerActor)
	{
		Player = Cast<ATopDownCharacter>(PlayerActor);
	}
	
	StartSpawning();
}

void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemySpawner::StartSpawning()
{
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &AEnemySpawner::OnSpawnTimerTimeout, SpawnTime, true, SpawnTime);
}

void AEnemySpawner::StopSpawning()
{
	GetWorldTimerManager().ClearTimer(SpawnTimer);
}

void AEnemySpawner::OnSpawnTimerTimeout()
{
	SpawnEnemy();
}

void AEnemySpawner::SpawnEnemy()
{
	// Calculate Spawn position
	FVector2D RandomPosition = FVector2D(FMath::VRand());
	RandomPosition.Normalize();
	RandomPosition *= SpawnDistance;

	// Spawn Enemy
	FVector EnemyLocation = GetActorLocation() + FVector(RandomPosition.X, 0.0f, RandomPosition.Y);
	AEnemy* Enemy = GetWorld()->SpawnActor<AEnemy>(EnemyActorToSpawn, EnemyLocation, FRotator::ZeroRotator);
	SetupEnemy(Enemy);
	
	// Increase difficulty
	TotalEnemyCount++;
	if (SpawnTime > SpawnTimeMinimumLimit && TotalEnemyCount % DifficultySpikeInterval == 0)
	{
		SpawnTime = FMath::Clamp(SpawnTime - DecreaseSpawnTimeByEveryInterval, SpawnTimeMinimumLimit, SpawnTime);
		
		StopSpawning();
		StartSpawning();
	}
}

void AEnemySpawner::SetupEnemy(AEnemy* Enemy)
{
	if (Enemy)
	{
		Enemy->Player = Player;
		Enemy->CanFollow = true;

		Enemy->EnemyDiedDelegate.AddDynamic(this, &AEnemySpawner::OnEnemyDied);
	}
}

void AEnemySpawner::OnEnemyDied()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("Enemy died"));
	//GunSurvivorsGameMode->AddScore();
}

