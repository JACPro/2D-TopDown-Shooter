#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Engine/TimerHandle.h"

#include "Enemy.h"
#include "TopDownCharacter.h"
#include "GunSurvivorsGameMode.h"

#include "EnemySpawner.generated.h"

UCLASS()
class GUNSURVIVORS_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AEnemy> EnemyActorToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnTime = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnDistance = 400.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int TotalEnemyCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int DifficultySpikeInterval = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnTimeMinimumLimit = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DecreaseSpawnTimeByEveryInterval = 0.05f;

	ATopDownCharacter* Player;

	AGunSurvivorsGameMode* GameMode;
	
	FTimerHandle SpawnTimer;

	AEnemySpawner();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void OnSpawnTimerTimeout();

	void StartSpawning();

	void StopSpawning();

	void SpawnEnemy();

	void SetupEnemy(AEnemy* Enemy);

	UFUNCTION()
	void OnEnemyDied();
};
