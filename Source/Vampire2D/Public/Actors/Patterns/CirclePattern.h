// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Patterns/CrossPattern.h"
#include "CirclePattern.generated.h"

/**
 * 
 */
UCLASS()
class VAMPIRE2D_API ACirclePattern : public ACrossPattern
{
	GENERATED_BODY()

public:
	ACirclePattern();

	virtual void Tick(float DeltaSeconds) override;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* ChildActor4;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* ChildActor5;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* ChildActor6;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* ChildActor7;

private:

public:
	void Initialize(int32 InSpecifiedID) override;
	
};
