// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Beam.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Connector.generated.h"

UCLASS()
class BALLPLATFORMER_API AConnector : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AConnector();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereConnector;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshConnector;

	UPROPERTY()
	TArray<UPhysicsConstraintComponent*> PhysicsConstraints;

	UPROPERTY()
	ABPGameMode* GameMode;
	
	UPROPERTY()
	float LinearBreakThreshold;

	UPROPERTY()
	bool LinearBreakable;

	UFUNCTION()
	void CheckOverlappingActor();

	UFUNCTION()
	void SetUpPhysicsConstraint(ABeam* Beam);

	UFUNCTION()
	bool HasConstraintWithBeam(ABeam* Beam) const;

	UFUNCTION()
	void HandleConstraintBroken(UPhysicsConstraintComponent* Constraint);
};
