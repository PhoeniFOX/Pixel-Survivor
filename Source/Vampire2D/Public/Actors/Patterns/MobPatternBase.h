// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MobPatternBase.generated.h"

UCLASS()
class VAMPIRE2D_API AMobPatternBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AMobPatternBase();

protected:
	virtual void BeginPlay() override;

	// Components
	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* RotationPlane;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* ChildActor;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* ChildActor1;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* ChildActor2;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* ChildActor3;

	// Properties
	UPROPERTY()
	TArray<UChildActorComponent*> SpecifiedLocationRefs;

	UPROPERTY()
	TArray<UChildActorComponent*> AdditionalRefs;

	int32 SpecifiedMobID = -1;

	// Functions
	bool AdjustSpawnLocation(const FVector& InOrigin, FVector& OutRandomLocation);
	

private:

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void Initialize(int32 InSpecifiedID);
};
