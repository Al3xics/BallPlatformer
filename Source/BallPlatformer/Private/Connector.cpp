// Fill out your copyright notice in the Description page of Project Settings.


#include "Connector.h"


// Sets default values
AConnector::AConnector()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	SphereConnector = CreateDefaultSubobject<USphereComponent>(TEXT("SphereConnector"));
	RootComponent = SphereConnector;
	SphereConnector->SetSphereRadius(256);
	SphereConnector->SetGenerateOverlapEvents(true);

	MeshConnector = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshConnector"));
	MeshConnector->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (MeshAsset.Succeeded())
	{
		MeshConnector->SetStaticMesh(MeshAsset.Object);
	}
	MeshConnector->SetWorldScale3D(FVector(4, 4, 4));
	MeshConnector->SetGenerateOverlapEvents(false);
	MeshConnector->CanCharacterStepUpOn = ECB_No;
	MeshConnector->SetCollisionProfileName(TEXT("NoCollision"));
}

// Called when the game starts or when spawned
void AConnector::BeginPlay()
{
	Super::BeginPlay();

	GameMode = Cast<ABPGameMode>(GetWorld()->GetAuthGameMode());
	LinearBreakThreshold = GameMode->LinearBreakThresholdConnector;
	LinearBreakable = GameMode->bLinearBreakableConnector;
}

// Called every frame
void AConnector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckOverlappingActor();
}

void AConnector::CheckOverlappingActor()
{
	TArray<AActor*> OverlappingActors;
	SphereConnector->GetOverlappingActors(OverlappingActors, ABeam::StaticClass());

	if (OverlappingActors.Num() > 0)
	{
		for (AActor* Actor : OverlappingActors)
		{
			ABeam* Beam = Cast<ABeam>(Actor);
			
			GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Yellow, FString::Printf(TEXT("Size : %i"), PhysicsConstraints.Num()));
			
			if (Beam && !HasConstraintWithBeam(Beam))
			{
				SetUpPhysicsConstraint(Beam);
			}
		}
	}
}

// Spawn and set up physics constraints to attach the connector and the beam
void AConnector::SetUpPhysicsConstraint(ABeam* Beam)
{
	if (!Beam) return;

	// Create a physical constraint attached to the Connector
	UPhysicsConstraintComponent* NewConstraint = NewObject<UPhysicsConstraintComponent>(this);
	if (!NewConstraint) return;

	NewConstraint->RegisterComponent();
	NewConstraint->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// Configure the constraint
	NewConstraint->SetConstrainedComponents(
		SphereConnector, NAME_None,	// The connector
		Beam->MeshBeam, NAME_None	// The Beam
	);
	NewConstraint->SetLinearBreakable(LinearBreakable, LinearBreakThreshold);

	PhysicsConstraints.Add(NewConstraint);
	Beam->PhysicsConstraints.Add(NewConstraint);
	Beam->ConstraintBrokenDelegate.AddUFunction(this, FName("HandleConstraintBroken"));
}

bool AConnector::HasConstraintWithBeam(ABeam* Beam) const
{
	if (!Beam) return false;
	
	for (UPhysicsConstraintComponent* Constraint : PhysicsConstraints)
	{
		if (Constraint)
		{
			UPrimitiveComponent* Comp1;
			UPrimitiveComponent* Comp2;
			FName Bone1, Bone2;

			// Get the components associated with this constraint
			Constraint->GetConstrainedComponents(Comp1, Bone1, Comp2, Bone2);

			// Compare components associated with the Beam actor
			if ((Comp1 == Beam->MeshBeam) || (Comp2 == Beam->MeshBeam))
			{
				return true;
			}
		}
	}
	return false;
}

void AConnector::HandleConstraintBroken(UPhysicsConstraintComponent* Constraint)
{
	Constraint->BreakConstraint();
	PhysicsConstraints.Remove(Constraint);
	Constraint->DestroyComponent();
}

