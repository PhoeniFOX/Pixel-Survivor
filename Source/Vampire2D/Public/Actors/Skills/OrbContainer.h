// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "OrbContainer.generated.h"

class AOrbBase;
class URotatingMovementComponent;

UCLASS()
class VAMPIRE2D_API AOrbContainer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOrbContainer();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* OrbRoot;
	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* OrbRotation;

	UPROPERTY(VisibleAnywhere)
	URotatingMovementComponent* RotatingMovementComponent;

	TArray<AOrbBase*> OrbArray;
	
	FRotator StartRotation;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	float RotationSpeed = -.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	TSubclassOf<AOrbBase> OrbClass;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	TSubclassOf<AOrbBase> SwordClass;

	FTimeline SwordRotationTimeline;


	
public:	
	void CreateOrbs(float InRadius, int32 InOrbNum, float InOrbScale, AActor* InTarget, float InAtkMultiplier, float InDamage, int32 InForce);
	void CreateSwords(float InRadius, int32 InOrbNum, float InOrbScale, AActor* InTarget, float InAtkMultiplier, float InDamage, int32 InForce);
	void SetupRotationSpeed(float InSpeed);
	
	UFUNCTION()
	void SwordRotationOverTime() const;
	
	TArray<AOrbBase*> GetOrbArray() const { return OrbArray; }
};
