// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "MovingPlatform.generated.h"

UCLASS()
class BALLPLATFORMER_API AMovingPlatform : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMovingPlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Platform")
	USplineComponent* SplineComponent;

	UPROPERTY(EditAnywhere, Category = "Platform")
	UStaticMeshComponent* PlatformMesh;

	UPROPERTY(EditAnywhere, Category = "Platform")
	UBoxComponent* BoxCollision;

	UPROPERTY(EditAnywhere, Category = "Platform")
	UCurveFloat* MovementCurve;

	UPROPERTY(EditAnywhere, Category = "Platform")
	bool bRotateObjectAlongSpline = true;

	UPROPERTY(EditAnywhere, Category = "Platform", meta = (ToolTip = "Multiplied by the velocity of the ball at the moment of impact."))
	float BounceFactor = 1.5f;

	FRotator InitialRotationOffset;
	
	FTimeline MovementTimeline;
	
	bool bIsLooping;
	
	float SplineLength;
	
	float CurrentDirection = 1.0f;

	UFUNCTION()
	void HandleProgress(float Value) const;

	UFUNCTION()
	void OnTimelineFinished();

	UFUNCTION()
	void BoxHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
