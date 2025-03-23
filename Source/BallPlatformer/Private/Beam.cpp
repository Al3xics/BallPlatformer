// Fill out your copyright notice in the Description page of Project Settings.


#include "Beam.h"

#include "Anchor.h"


// Sets default values
ABeam::ABeam()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	RootComponent = BoxCollision;
	BoxCollision->SetBoxExtent(FVector(800, 100, 100));
	BoxCollision->SetGenerateOverlapEvents(true);
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ABeam::OnOverlapBegin);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &ABeam::OnOverlapEnd);

	MeshBeam = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Beam"));
	MeshBeam->SetupAttachment(RootComponent);
	MeshBeam->SetGenerateOverlapEvents(false);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (MeshAsset.Succeeded())
	{
		MeshBeam->SetStaticMesh(MeshAsset.Object);
	}
	MeshBeam->SetWorldScale3D(FVector(16, 2, 2));
	MeshBeam->SetSimulatePhysics(true);
}

// Called when the game starts or when spawned
void ABeam::BeginPlay()
{
	Super::BeginPlay();

	GameMode = Cast<ABPGameMode>(GetWorld()->GetAuthGameMode());
}

// Called every frame
void ABeam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckForBreakage();
}

void ABeam::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || !OtherComp) return;

	if (OtherComp && OtherComp->IsSimulatingPhysics())
	{
		const float Mass = OtherComp->GetMass();
		const FVector Velocity = OtherActor->GetVelocity();
		const float Speed = Velocity.Size();
		const float KineticEnergy = 0.5f * Mass * Speed * Speed;

		CurrentLoad += Mass * KineticEnergy * 0.01f;
	}
}

void ABeam::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor || !OtherComp) return;

	if (OtherComp && OtherComp->IsSimulatingPhysics())
	{
		const float Mass = OtherComp->GetMass();
		const FVector Velocity = OtherActor->GetVelocity();
		const float Speed = Velocity.Size();
		const float KineticEnergy = 0.5f * Mass * Speed * Speed;

		CurrentLoad -= Mass * KineticEnergy * 0.01f;
	}
}

void ABeam::CheckForBreakage()
{
	if (CurrentLoad > GameMode->MaxLoadPerBeam)
	{
		// Break all constraints related to this beam
		for (UPhysicsConstraintComponent* Constraint : PhysicsConstraints)
		{
			if (Constraint)
			{
				// Checks the actors attached to both ends of the constraint
				const AActor* Actor1 = Constraint->ConstraintActor1;
				const AActor* Actor2 = Constraint->ConstraintActor2;

				// If one end is an anchor, do not break this constraint
				if (Actor1 && Actor1->IsA(AAnchor::StaticClass()) ||
					Actor2 && Actor2->IsA(AAnchor::StaticClass()))
				{
					continue;
				}

				// It's not an anchor, so break the constraint and destroy it
				PhysicsConstraints.Remove(Constraint);
				ConstraintBrokenDelegate.Broadcast(Constraint);
			}
		}

		// Destroy beam after X seconds
		SetLifeSpan(GameMode->DestroyBeamAfterXSeconds);
	}
}

