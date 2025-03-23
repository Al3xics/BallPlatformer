// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BPGameMode.generated.h"


UCLASS()
class BALLPLATFORMER_API ABPGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="BP Game Mode")
	float MaxLoadPerBeam = 500.f;
	
	UPROPERTY(EditAnywhere, Category="BP Game Mode")
	float DestroyBeamAfterXSeconds = 5.f;
	
	UPROPERTY(EditAnywhere, Category="BP Game Mode")
	bool bLinearBreakableAnchor = false;
	
	UPROPERTY(EditAnywhere, Category="BP Game Mode", meta=(EditCondition = "bLinearBreakableAnchor"))
	float LinearBreakThresholdAnchor = 50000000.f;
	
	UPROPERTY(EditAnywhere, Category="BP Game Mode")
	bool bLinearBreakableConnector = true;
	
	UPROPERTY(EditAnywhere, Category="BP Game Mode", meta=(EditCondition = "bLinearBreakableConnector"))
	float LinearBreakThresholdConnector = 5000000.f;
};
