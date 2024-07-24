// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Indicator.generated.h"

class UPaperSpriteComponent;

UCLASS()
class VAMPIRE2D_API AIndicator : public AActor
{
	GENERATED_BODY()
	
public:	
	AIndicator();
	
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere)
	UPaperSpriteComponent* LineSprite;

public:
	
};
