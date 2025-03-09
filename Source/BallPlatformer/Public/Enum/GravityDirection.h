// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GravityDirection.generated.h"

/**
 * 
 */
UENUM()
enum EGravityDirection
{
	Left,
	Right,
	Top,
	Bottom,
	Front,
	Back,
};
