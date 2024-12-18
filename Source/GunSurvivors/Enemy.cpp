#include "Enemy.h"

#include "Kismet/GameplayStatics.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	SetRootComponent(CapsuleComponent);

	EnemyFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("EnemyFlipbook"));
	EnemyFlipbook->SetupAttachment(RootComponent);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	if (!Player)
	{
		AActor* PlayerActor = UGameplayStatics::GetActorOfClass(GetWorld(), ATopDownCharacter::StaticClass());
		if (PlayerActor)
		{
			Player = Cast<ATopDownCharacter>(PlayerActor);
			CanFollow = true;
		}
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsAlive && CanFollow && Player) 
	{
		// Move towards player
		FVector CurrentLocation = GetActorLocation();
		FVector PlayerLocation = Player->GetActorLocation();

		FVector DirectionToPlayer = PlayerLocation - CurrentLocation;
		float DistanceToPlayer = DirectionToPlayer.Length();

		if (DistanceToPlayer > StopDistance)
		{
			DirectionToPlayer.Normalize();
			FVector NewLocation = CurrentLocation + DirectionToPlayer * MovementSpeed * DeltaTime;
			SetActorLocation(NewLocation);
		}

		// Face player
		CurrentLocation = GetActorLocation();
		float FlipbookXScale = EnemyFlipbook->GetComponentScale().X;

		if (PlayerLocation.X - CurrentLocation.X >= 0.0f) // Player to right
		{
			if (FlipbookXScale < 0.0f)
			{
				EnemyFlipbook->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
			}
		}
		else // Player to left
		{
			if (FlipbookXScale > 0.0f)
			{
				EnemyFlipbook->SetWorldScale3D(FVector(-1.0f, 1.0f, 1.0f));
			}
		}
	}
}

