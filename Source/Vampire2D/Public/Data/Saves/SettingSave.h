// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SettingSave.generated.h"

/**
 * 
 */
UCLASS()
class VAMPIRE2D_API USettingSave : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	float BGMVolumes = 1.f;

	UPROPERTY()
	float SFXVolumes = 1.f;
};
