// Fill out your copyright notice in the Description page of Project Settings.


#include "BallPlatformer/Public/BPBall.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ABPBall::ABPBall()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	RootComponent = SphereComponent;
	SphereComponent->SetSimulatePhysics(true);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComponent->SetNotifyRigidBodyCollision(true);
	SphereComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	SphereComponent->SetMassScale(NAME_None, 1.0f);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 800.f;  // Distance de la caméra
	SpringArmComponent->bUsePawnControlRotation = true;  // Rotation contrôlée par le Pawn

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
}

// Called when the game starts or when spawned
void ABPBall::BeginPlay()
{
	Super::BeginPlay();
	
	SphereComponent->OnComponentHit.AddDynamic(this, &ABPBall::OnSphereHit);

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

void ABPBall::Destroyed()
{
	Super::Destroyed();

	UGameplayStatics::OpenLevel(GetWorld(), FName(*GetWorld()->GetMapName()));
}

// Called every frame
void ABPBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector GravityForceVector = CurrentGravityDirection * GravityStrength * SphereComponent->GetMass();
	SphereComponent->AddForce(GravityForceVector);
}

// Called to bind functionality to input
void ABPBall::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABPBall::MoveFunc);
	EnhancedInput->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ABPBall::JumpFunc);
	EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABPBall::LookFunc);
}

void ABPBall::MoveFunc(const FInputActionValue& Value)
{
	const FVector MovementVector = Value.Get<FVector>();

	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		const FRotator ControlRotation = PlayerController->GetControlRotation();
        
		FVector ForwardDirection = FRotationMatrix(ControlRotation).GetScaledAxis(EAxis::X);
		FVector RightDirection = FRotationMatrix(ControlRotation).GetScaledAxis(EAxis::Y);

		ForwardDirection.Z = 0.0f;
		RightDirection.Z = 0.0f;

		ForwardDirection.Normalize();
		RightDirection.Normalize();

		const FVector Movement = (ForwardDirection * MovementVector.X + RightDirection * MovementVector.Y) * MoveForce;

		SphereComponent->AddForce(Movement);
		// SphereComponent->SetPhysicsLinearVelocity(Movement, bCumulateSpeed);
	}
}

void ABPBall::JumpFunc(const FInputActionValue& Value)
{
	const bool JumpValue = Value.Get<bool>();

	if (bIsOnGround || JumpCount < MaxJumps) 
	{
		SphereComponent->AddImpulse(JumpImpulse * FVector::UpVector);
		bIsOnGround = false;
		JumpCount++;
	}
}

void ABPBall::LookFunc(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);  // Rotation horizontale
	AddControllerPitchInput(-LookAxisVector.Y); // Rotation verticale (inverse pour correspondre aux contrôles classiques)
}

void ABPBall::OnSphereHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (FMath::Abs(Hit.Normal.Z) > 0.7)
	{
		JumpCount = 0;
		bIsOnGround = true;
	}
}

void ABPBall::SetGravityDirection(const FVector& NewGravityDirection, const float NewGravityStrength)
{
	CurrentGravityDirection = NewGravityDirection.GetSafeNormal();
	GravityStrength = NewGravityStrength;
}
