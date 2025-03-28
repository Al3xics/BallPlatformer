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
	BoxCollision->SetSimulatePhysics(true);
	BoxCollision->SetGenerateOverlapEvents(true);
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BoxCollision->SetCollisionResponseToAllChannels(ECR_Overlap);

	MeshBeam = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Beam"));
	MeshBeam->SetupAttachment(RootComponent);
	MeshBeam->SetGenerateOverlapEvents(false);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (MeshAsset.Succeeded())
	{
		MeshBeam->SetStaticMesh(MeshAsset.Object);
	}
	MeshBeam->SetWorldScale3D(FVector(16, 2, 2));
	MeshBeam->SetSimulatePhysics(false);
	MeshBeam->SetCollisionProfileName(TEXT("BlockAllDynamic"));
}

// Called when the game starts or when spawned
void ABeam::BeginPlay()
{
	Super::BeginPlay();

	GameMode = Cast<ABPGameMode>(GetWorld()->GetAuthGameMode());
	
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ABeam::OnOverlapBegin);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &ABeam::OnOverlapEnd);
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
	if (Cast<AConnector>(OtherActor) || Cast<ABeam>(OtherActor)) return;

	if (OtherComp && OtherComp->IsSimulatingPhysics())
	{
		const float Mass = OtherComp->GetMass();
		const FVector Velocity = OtherActor->GetVelocity();
		const float Speed = Velocity.Size();
		const float KineticEnergy = 0.5f * Mass * Speed * Speed;

		CurrentLoad += Mass * KineticEnergy * 0.0000001f;
	}
}

void ABeam::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor || !OtherComp) return;
	if (Cast<AConnector>(OtherActor) || Cast<ABeam>(OtherActor)) return;

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
	if (bIsBroken) return;
	
	if (CurrentLoad > GameMode->MaxLoadPerBeam)
	{
		bIsBroken = true;
		
		// Temporary list to store constraints to be deleted
		TArray<UPhysicsConstraintComponent*> ConstraintsToRemove;
		
		// Break all constraints related to this beam
		for (UPhysicsConstraintComponent* Constraint : PhysicsConstraints)
		{
			if (Constraint)
			{
				UPrimitiveComponent* Comp1;
				UPrimitiveComponent* Comp2;
				FName Bone1, Bone2;

				Constraint->GetConstrainedComponents(Comp1, Bone1, Comp2, Bone2);

				// If one end is an anchor, do not break this constraint
				if (Comp1->GetOwner() && Comp1->GetOwner()->IsA(AAnchor::StaticClass()) ||
					Comp2->GetOwner() && Comp2->GetOwner()->IsA(AAnchor::StaticClass()))
				{
					continue;
				}

				ConstraintsToRemove.Add(Constraint);
			}
		}

		for (UPhysicsConstraintComponent* Constraint : ConstraintsToRemove)
		{
			PhysicsConstraints.Remove(Constraint);
			BoxCollision->SetGenerateOverlapEvents(false);
				
			if (AConnector* Connector = Cast<AConnector>(Constraint->GetOwner()))
				Connector->HandleConstraintBroken(Constraint);

			// If Beam is directly in contact with another Beam, it will not fall because it's a Physics Actor, and the other Beam prevent him from falling down
			MeshBeam->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetWorld()->GetTimerManager().SetTimer(TimerHandleReEnableCollision, this, &ABeam::ReEnableCollision, 0.7f, false);


		}
	}
}

void ABeam::ReEnableCollision() const
{
	MeshBeam->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

