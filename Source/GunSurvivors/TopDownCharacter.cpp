#include "TopDownCharacter.h"

ATopDownCharacter::ATopDownCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapsuleComp);

	CharacterFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("CharacterFlipbook"));
	CharacterFlipbook->SetupAttachment(RootComponent);

}

void ATopDownCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = 
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

}

void ATopDownCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

