// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Enum/GravityDirection.h"
#include "GameFramework/Actor.h"
#include "GravityDoor.generated.h"

UCLASS()
class BALLPLATFORMER_API AGravityDoor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGravityDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneRoot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UArrowComponent* GravityDirectionArrow;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DoorFrameRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DoorFrameLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DoorFrameTop;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* PassTroughDoor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gravity")
	TEnumAsByte<EGravityDirection> GravityDirection = Top;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gravity")
	float GravityForce = 1470.f;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	FVector GetGravityDirectionVector() const;

	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
