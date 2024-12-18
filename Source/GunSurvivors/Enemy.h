#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/CapsuleComponent.h"
#include "PaperFlipbookComponent.h"

#include "TopDownCharacter.h"

#include "Engine/TimerHandle.h"

#include "Enemy.generated.h"

UCLASS()
class GUNSURVIVORS_API AEnemy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Collision
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCapsuleComponent* CapsuleComponent;

	// Animation
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UPaperFlipbookComponent* EnemyFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperFlipbook* DeadFlipbookAsset;

	// Player reference
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ATopDownCharacter* Player;

	// Gameplay
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsAlive = true;

	// Movement
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool CanFollow = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeed = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StopDistance = 20.0f;

	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DestroyTime = 10.0f;


	AEnemy();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	void Die();

	void OnDestroyTimerTimeout();
};
