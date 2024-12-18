#include "Bullet.h"

#include "Enemy.h"

ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SetRootComponent(SphereComponent);

	BulletSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("BulletSprite"));
	BulletSprite->SetupAttachment(RootComponent);

	MovementDirection = FVector2D(1.0f, 0.0f);	
}

void ABullet::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OverlapBegin);
}

void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handle Movement
	if (IsLaunched)
	{
		FVector2D DistanceToMove = MovementDirection * MovementSpeed * DeltaTime;

		FVector CurrentLocation = GetActorLocation();
		FVector NewLocation = CurrentLocation + FVector(DistanceToMove.X, 0.0f, DistanceToMove.Y);

		SetActorLocation(NewLocation);
	}
}

void ABullet::Launch(FVector2D Direction, float Speed)
{
	if (IsLaunched) return;

	IsLaunched = true;

	MovementDirection = Direction;
	MovementSpeed = Speed;

	GetWorldTimerManager().SetTimer(DeleteTimer, this, &ABullet::OnDeleteTimerTimeout, 1.0f, false, DeleteTime);
}

void ABullet::OnDeleteTimerTimeout()
{
	Destroy();
}

void ABullet::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult)
{
	AEnemy* Enemy = Cast<AEnemy>(OtherActor);

	if (Enemy && Enemy->IsAlive)
	{
		DisableBullet();
		Enemy->Die();
	}
}

void ABullet::DisableBullet()
{
	if (IsDisabled) return;

	IsDisabled = true;
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BulletSprite->DestroyComponent();
}
