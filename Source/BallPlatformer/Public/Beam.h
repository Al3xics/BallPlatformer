// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BPGameMode.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Beam.generated.h"

UCLASS()
class BALLPLATFORMER_API ABeam : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABeam();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxCollision;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshBeam;

	UPROPERTY()
	TArray<UPhysicsConstraintComponent*> PhysicsConstraints;

	UPROPERTY()
	float CurrentLoad = 0.f;

	UPROPERTY()
	ABPGameMode* GameMode;

	UPROPERTY()
	bool bIsBroken = false;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void CheckForBreakage();
};
