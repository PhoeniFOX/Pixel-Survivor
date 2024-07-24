// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Patterns/MobPatternBase.h"
#include "GridPattern.generated.h"

/**
 * 
 */
UCLASS()
class VAMPIRE2D_API AGridPattern : public AMobPatternBase
{
	GENERATED_BODY()
	
public:
	AGridPattern();
	
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

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* ChildActor8;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* ChildActor9;

private:
	int32 SpawnIterator;
	int32 MaxLoop;

	FTimerHandle KeepSpawnTimerHandle;

	void SpawnGrid();
public:
	void PreInitialize(int32 InMaxLoop);
	virtual void Initialize(int32 InSpecifiedID) override;
};
