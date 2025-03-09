// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityDoor.h"

#include "BPBall.h"


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
}

FVector AGravityDoor::GetGravityDirectionVector() const
{
	FVector LocalDirection;
    
    switch (GravityDirection)
    {
        case Top:
            LocalDirection = FVector(0, 0, 1);
            break;
        case Bottom:
            LocalDirection = FVector(0, 0, -1);
            break;
        case Left:
            LocalDirection = FVector(0, -1, 0);
            break;
        case Right:
            LocalDirection = FVector(0, 1, 0);
            break;
        case Front:
            LocalDirection = FVector(1, 0, 0);
            break;
        case Back:
            LocalDirection = FVector(-1, 0, 0);
            break;
        default:
            LocalDirection = FVector(0, 0, -1);
            break;
    }

    // Convertir la direction locale en direction du monde en utilisant l'orientation de la porte
    return GetActorTransform().TransformVectorNoScale(LocalDirection).GetSafeNormal();
}

void AGravityDoor::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABPBall* Ball = Cast<ABPBall>(OtherActor);
	
	if (Ball)
	{
		Ball->SetGravityDirection(GetGravityDirectionVector(), GravityForce);
	}
}
