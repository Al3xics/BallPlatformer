// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "BPBall.generated.h"

UCLASS()
class BALLPLATFORMER_API ABPBall : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABPBall();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TSoftObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category="Movement")
	float MoveForce = 600.f;

	UPROPERTY(EditAnywhere, Category="Movement")
	float JumpImpulse = 800.f;

	UPROPERTY(VisibleAnywhere, Category="Movement")
	int JumpCount = 0;
	
	UPROPERTY(EditAnywhere, Category="Movement")
	int MaxJumps = 2;
	
	// UPROPERTY(EditAnywhere, Category="Movement")
	// bool bCumulateSpeed = false;

	bool bIsOnGround = false;

	UFUNCTION()
	void MoveFunc(const FInputActionValue& Value);

	UFUNCTION()
	void JumpFunc(const FInputActionValue& Value);

	UFUNCTION()
	void LookFunc(const FInputActionValue& Value);

	UFUNCTION()
	void OnSphereHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
