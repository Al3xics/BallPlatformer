// Fill out your copyright notice in the Description page of Project Settings.


#include "BallPlatformer/Public/BPBall.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


// Sets default values
ABPBall::ABPBall()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	RootComponent = SphereComponent;
	SphereComponent->SetSimulatePhysics(true);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABPBall::BeginPlay()
{
	Super::BeginPlay();

	// Add Mapping Contexte
    if (const auto PlayerController = Cast<APlayerController>(Controller))
    {
		if (UEnhancedInputLocalPlayerSubsystem* PlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
     	{
     		if (!MappingContext.IsNull())
     		{
				PlayerSubsystem->AddMappingContext(MappingContext.LoadSynchronous(), 0);
     		}
     	}
    }
}

// Called every frame
void ABPBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABPBall::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABPBall::MoveFunc);
	EnhancedInput->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ABPBall::JumpFunc);
}

void ABPBall::MoveFunc(const FInputActionValue& Value)
{
	const FVector MovementVector = Value.Get<FVector>();
	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Cyan, TEXT("Move Action"));
	SphereComponent->AddForce(MoveForce * MovementVector);
}

void ABPBall::JumpFunc(const FInputActionValue& Value)
{
	const bool JumpValue = Value.Get<bool>();
	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Cyan, TEXT("Jump Action"));
	SphereComponent->AddImpulse(JumpImpulse * GetActorUpVector());
}

