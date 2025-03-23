// Fill out your copyright notice in the Description page of Project Settings.


#include "Anchor.h"


// Sets default values
AAnchor::AAnchor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAnchor::BeginPlay()
{
	Super::BeginPlay();

	LinearBreakThreshold = GameMode->LinearBreakThresholdAnchor;
	LinearBreakable = GameMode->bLinearBreakableAnchor;
}

// Called every frame
void AAnchor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

