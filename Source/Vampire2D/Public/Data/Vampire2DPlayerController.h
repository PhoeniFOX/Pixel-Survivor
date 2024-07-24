// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Vampire2DPlayerController.generated.h"

class UGameLayout;
/**
 * 
 */
class UVampireGameInstance;

UCLASS()
class VAMPIRE2D_API AVampire2DPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AVampire2DPlayerController();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UVampireGameInstance* VampireGameInstance;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameLayoutClass;

	UPROPERTY()
	UGameLayout* GameLayout;

public:
	void CreateLayout();
	UGameLayout* GetGameLayout();
};
