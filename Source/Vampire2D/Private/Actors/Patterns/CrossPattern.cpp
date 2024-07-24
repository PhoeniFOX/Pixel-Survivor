// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Patterns/CrossPattern.h"

#include "Actors/LevelManager.h"
#include "Data/GameInstanceFunctionLibrary.h"
#include "Data/VampireGameInstance.h"

ACrossPattern::ACrossPattern()
{
	
}

void ACrossPattern::BeginPlay()
{
	Super::BeginPlay();

}

void ACrossPattern::Initialize(int32 InSpecifiedID)
{
	Super::Initialize(InSpecifiedID);
	
	SpecifiedLocationRefs.Add(ChildActor);
	SpecifiedLocationRefs.Add(ChildActor1);
	SpecifiedLocationRefs.Add(ChildActor2);
	SpecifiedLocationRefs.Add(ChildActor3);

	for (UChildActorComponent* ActorComponent : SpecifiedLocationRefs)
	{
		FVector SpawnLocation;
		if (AdjustSpawnLocation(ActorComponent->GetComponentLocation(), SpawnLocation))
		{
			UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->LevelManager->PoolingMobs(
				false,
				SpecifiedMobID,
				ActorComponent->GetComponentLocation(),
				true);
		}
	}
}

void ACrossPattern::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}
