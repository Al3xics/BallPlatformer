// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityDoor.h"


// Sets default values
AGravityDoor::AGravityDoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("/Engine/BasicShapes/Cylinder"));


	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	GravityDirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("GravityDirectionArrow"));
	GravityDirectionArrow->SetWorldLocation(FVector(0, 0, 170));
	GravityDirectionArrow->SetupAttachment(RootComponent);

	DoorFrameRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrameRight"));
	DoorFrameRight->SetStaticMesh(MeshObj.Object);
	DoorFrameRight->SetRelativeLocation(FVector(0, -150, 200));
	DoorFrameRight->SetRelativeScale3D(FVector(1, 0.5, 4));
	DoorFrameRight->SetupAttachment(RootComponent);

	DoorFrameLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrameLeft"));
	DoorFrameLeft->SetStaticMesh(MeshObj.Object);
	DoorFrameLeft->SetRelativeLocation(FVector(0, 150, 200));
	DoorFrameLeft->SetRelativeScale3D(FVector(1, 0.5, 4));
	DoorFrameLeft->SetupAttachment(RootComponent);

	DoorFrameTop = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrameTop"));
	DoorFrameTop->SetStaticMesh(MeshObj.Object);
	DoorFrameTop->SetRelativeLocation(FVector(0, 0, 350));
	DoorFrameTop->SetWorldRotation(FRotator(90, 0, 0));
	DoorFrameTop->SetRelativeScale3D(FVector(1, 0.5, 4));
	DoorFrameTop->SetupAttachment(RootComponent);

	PassTroughDoor = CreateDefaultSubobject<UBoxComponent>(TEXT("PassTroughDoor"));
	PassTroughDoor->SetWorldLocation(FVector(0, 0, 170));
	PassTroughDoor->SetBoxExtent(FVector(40, 135, 170));
	PassTroughDoor->SetupAttachment(RootComponent);
	PassTroughDoor->OnComponentBeginOverlap.AddDynamic(this, &AGravityDoor::OnBoxBeginOverlap);
}

// Called when the game starts or when spawned
void AGravityDoor::BeginPlay()
{
	Super::BeginPlay();
}

void AGravityDoor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	FVector GravityDirectionVector = GetGravityDirectionVector();
	FRotator ArrowRotation = GravityDirectionVector.Rotation();
	GravityDirectionArrow->SetWorldRotation(ArrowRotation);
}

// Called every frame
void AGravityDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsGravityOn)
	{
		LaunchGravity();
	}
}

FVector AGravityDoor::GetGravityDirectionVector() const
{
	switch (GravityDirection)
	{
		case Top:
			return FVector(0, 0, 1); // Gravité vers le haut
		case Bottom:
			return FVector(0, 0, -1); // Gravité vers le bas
		case Left:
			return FVector(-1, 0, 0); // Gravité vers la gauche
		case Right:
			return FVector(1, 0, 0); // Gravité vers la droite
		case Front:
			return FVector(0, 1, 0); // Gravité vers l'avant
		case Back:
			return FVector(0, -1, 0); // Gravité vers l'arrière
		default:
			return FVector(0, 0, -1); // Par défaut, la gravité pointe vers le bas
	}
}

void AGravityDoor::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AffectedComponent = Cast<UPrimitiveComponent>(OtherComp);
		if (AffectedComponent && AffectedComponent->IsSimulatingPhysics())
		{
			bIsGravityOn = true;
		}
	}
}

void AGravityDoor::LaunchGravity()
{
	if (GravityDirection == Bottom)
	{
		bIsGravityOn = false;
		return;
	}
	
	FVector GravityDirectionVector = GetGravityDirectionVector();
	FVector NewVelocity = GravityDirectionVector * GravityForce;

	AffectedComponent->SetPhysicsLinearVelocity(NewVelocity, false);
}

