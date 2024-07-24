// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Patterns/MobPatternBase.h"
#include "CrossPattern.generated.h"

/**
 * 
 */
UCLASS()
class VAMPIRE2D_API ACrossPattern : public AMobPatternBase
{
	GENERATED_BODY()

public:
	ACrossPattern();
	
	virtual void Tick(float DeltaSeconds) override;
protected:
	virtual void BeginPlay() override;

private:

public:
	virtual void Initialize(int32 InSpecifiedID) override;

};

