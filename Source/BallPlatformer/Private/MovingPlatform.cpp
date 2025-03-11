// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"

#include "BPBall.h"


// Sets default values
AMovingPlatform::AMovingPlatform()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	RootComponent = SplineComponent;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	PlatformMesh->SetupAttachment(RootComponent);
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (MeshAsset.Succeeded())
	{
		PlatformMesh->SetStaticMesh(MeshAsset.Object);
		PlatformMesh->SetWorldScale3D(FVector(4.f, 4.f, 0.25f));
		PlatformMesh->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
		PlatformMesh->SetNotifyRigidBodyCollision(true);
		PlatformMesh->SetCollisionProfileName(TEXT("BlockAll"));
	}
}

// Called when the game starts or when spawned
void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	PlatformMesh->OnComponentHit.AddDynamic(this, &AMovingPlatform::BoxHit);

	SplineLength = SplineComponent->GetSplineLength();
	bIsLooping = SplineComponent->IsClosedLoop();

	const FRotator InitialSplineRotation = SplineComponent->GetRotationAtDistanceAlongSpline(0.f, ESplineCoordinateSpace::World);
	InitialRotationOffset = PlatformMesh->GetComponentRotation() - InitialSplineRotation;

	if (MovementCurve)
	{
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("HandleProgress"));

		FOnTimelineEvent FinishedFunction;
		FinishedFunction.BindUFunction(this, FName("OnTimelineFinished"));

		MovementTimeline.AddInterpFloat(MovementCurve, ProgressFunction);
		MovementTimeline.SetTimelineFinishedFunc(FinishedFunction);

		MovementTimeline.PlayFromStart();
	}
}

// Called every frame
void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MovementTimeline.TickTimeline(DeltaTime);
}

void AMovingPlatform::HandleProgress(const float Value) const
{
	const float Distance = Value * SplineLength;
	if (bRotateObjectAlongSpline)
	{
		FTransform SplineTransform = SplineComponent->GetTransformAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
		PlatformMesh->SetWorldLocation(SplineTransform.GetLocation());
		const FRotator SplineRotation = SplineComponent->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);

		// Appliquer la rotation initiale de l'actor pour qu'il garde son orientation locale
		const FRotator FinalRotation = SplineRotation + InitialRotationOffset;
		PlatformMesh->SetWorldRotation(FinalRotation);
	}
	else
	{
		const FVector NewLocation = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
		PlatformMesh->SetWorldLocation(NewLocation);
	}
}

void AMovingPlatform::OnTimelineFinished()
{
	if (bIsLooping)
	{
		MovementTimeline.PlayFromStart();
	}
	else
	{
		MovementTimeline.SetPlayRate(-MovementTimeline.GetPlayRate());
		MovementTimeline.Play();
	}
}

void AMovingPlatform::BoxHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ABPBall* Ball = Cast<ABPBall>(OtherActor))
	{
		FVector BallVelocity = Ball->SphereComponent->GetPhysicsLinearVelocity();
		FVector ReflectionDirection = BallVelocity.MirrorByVector(Hit.Normal);
		FVector Impulse = ReflectionDirection * BounceFactor * Ball->SphereComponent->GetMass();
		
		Ball->SphereComponent->AddImpulse(Impulse, NAME_None, true);
	}
}

