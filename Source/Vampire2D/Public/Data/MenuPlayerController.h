// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MenuPlayerController.generated.h"

class URegisterPlayerName;
class UMenu;
/**
 * 
 */
UCLASS()
class VAMPIRE2D_API AMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void CreateMenuUI();
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category= "UI")
	TSubclassOf<UMenu> MenuWidgetClass;

	// UPROPERTY(EditDefaultsOnly, Category= "UI")
	// TSubclassOf<URegisterPlayerName> RegisterPlayerNameWidgetClass;

	// void DelayAfterCreateMenu();
};
