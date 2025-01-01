#include "TopDownCharacter.h"

#include "Kismet/KismetMathLibrary.h"
#include "Enemy.h"

ATopDownCharacter::ATopDownCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapsuleComp);

	CharacterFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("CharacterFlipbook"));
	CharacterFlipbook->SetupAttachment(RootComponent);

	GunParent = CreateDefaultSubobject< USceneComponent>(TEXT("GunParent"));	
	GunParent->SetupAttachment(RootComponent);

	GunSprite = CreateDefaultSubobject< UPaperSpriteComponent>(TEXT("GunSprite"));
	GunSprite->SetupAttachment(GunParent);

	BulletSpawnPosition = CreateDefaultSubobject<USceneComponent>(TEXT("BulletSpawnPosition"));
	BulletSpawnPosition->SetupAttachment(GunSprite);
}

void ATopDownCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		PlayerController->SetShowMouseCursor(true);

		UEnhancedInputLocalPlayerSubsystem* Subsystem = 
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &ATopDownCharacter::OverlapBegin);
}

void ATopDownCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handle Movement
	if (CanMove)
	{
		if (MovementDirection.Length() > 0.0f) // some movement key pressed
		{
			MovementDirection.Normalize();

			FVector2D DistanceToMove = MovementDirection * MovementSpeed * DeltaTime;

			FVector CurrentLocation = GetActorLocation();
			FVector NewLocation = CurrentLocation + FVector(DistanceToMove.X, 0.0f, 0.0f);
			if (!IsInMapBoundsHorizontal(NewLocation.X))
			{
				NewLocation -= FVector(DistanceToMove.X, 0.0f, 0.0f);
			}

			NewLocation += FVector(0.0f, 0.0f, DistanceToMove.Y);
			if (!IsInMapBoundsVertical(NewLocation.Z))
			{
				NewLocation -= FVector(0.0f, 0.0f, DistanceToMove.Y);
			}

			SetActorLocation(NewLocation);
		}
	}

	// Handle Gun Rotation
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		FVector MouseWorldLocation, MouseWorldDirection;
		PlayerController->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection);

		FVector CurrentLocation = GetActorLocation();
		FVector Start = FVector(CurrentLocation.X, 0.0f, CurrentLocation.Z);
		FVector Target = FVector(MouseWorldLocation.X, 0.0f, MouseWorldLocation.Z);
		FRotator GunParentRotator = UKismetMathLibrary::FindLookAtRotation(Start, Target);

		GunParent->SetRelativeRotation(GunParentRotator);
	}
}

// Called to bind functionality to input
void ATopDownCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATopDownCharacter::MoveTriggered);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ATopDownCharacter::MoveCompleted);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Canceled, this, &ATopDownCharacter::MoveCompleted);

		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ATopDownCharacter::Shoot);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &ATopDownCharacter::Shoot);
	}
}

void ATopDownCharacter::MoveTriggered(const FInputActionValue& Value)
{
	FVector2D MoveActionValue = Value.Get<FVector2D>();
	
	if (CanMove)
	{
		MovementDirection = MoveActionValue;
		CharacterFlipbook->SetFlipbook(RunFlipbook);
		
		FVector FlipbookScale = CharacterFlipbook->GetComponentScale();
		if (MovementDirection.X < 0)
		{
			if (FlipbookScale.X > 0.0f) 
			{
				CharacterFlipbook->SetWorldScale3D(FVector(-1.0f, 1.0f, 1.0f));
			}
		}
		else if (MovementDirection.X > 0)
		{
			if (FlipbookScale.X < 0.0f)
			{
				CharacterFlipbook->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
			}
		}
	}
}

void ATopDownCharacter::MoveCompleted(const FInputActionValue& Value)
{
	MovementDirection = FVector2D::ZeroVector;
	CharacterFlipbook->SetFlipbook(IdleFlipbook);
}

void ATopDownCharacter::Shoot(const FInputActionValue& Value)
{
	if (!CanShoot) return;

	CanShoot = false;

	// Spawn Bullet Actor
	ABullet* Bullet = GetWorld()->SpawnActor<ABullet>(BulletActorToSpawn, BulletSpawnPosition->GetComponentLocation(), FRotator::ZeroRotator);
	check(Bullet); // Debugging assertion

	// Get Mouse World Location
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	check(PlayerController); // Debugging assertion
	FVector MouseWorldLocation, MouseWorldDirection;
	PlayerController->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection);

	// Calculate Bullet Direction
	FVector CurrentLocation = GetActorLocation();
	FVector2D BulletDirection = FVector2D(MouseWorldLocation.X - CurrentLocation.X, MouseWorldLocation.Z - CurrentLocation.Z);
	BulletDirection.Normalize();

	// Launch the bullet
	float BulletSpeed = 250.0f;
	Bullet->Launch(BulletDirection, BulletSpeed);

	// Set Destroy Timer
	GetWorldTimerManager().SetTimer(ShootCooldownTimer, this, &ATopDownCharacter::OnShootCooldownTimerTimeout, 1.0f, false, ShootCooldownDurationInSeconds);
}

bool ATopDownCharacter::IsInMapBoundsHorizontal(float XPos)
{
	bool Result = true;

	Result = (XPos > HorizontalLimits.X && XPos < HorizontalLimits.Y);

	return Result;
}

bool ATopDownCharacter::IsInMapBoundsVertical(float ZPos)
{
	bool Result = true;

	Result = (ZPos > VerticalLimits.X && ZPos < VerticalLimits.Y);

	return Result;
}

void ATopDownCharacter::OnShootCooldownTimerTimeout()
{
	CanShoot = true;
}

void ATopDownCharacter::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult)
{
	AEnemy* Enemy = Cast<AEnemy>(OtherActor);

	if (Enemy && Enemy->IsAlive)
	{
		if (IsAlive)
		{
			IsAlive = false;
			CanMove = false;
			CanShoot = false;

			PlayerDiedDelegate.Broadcast();
		}
	}
}

