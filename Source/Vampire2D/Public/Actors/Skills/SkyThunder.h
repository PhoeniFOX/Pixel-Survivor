// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkyThunder.generated.h"

class APlayerCharacter;
class UBoxComponent;

UCLASS()
class VAMPIRE2D_API ASkyThunder : public AActor
{
	GENERATED_BODY()
	
public:	
	ASkyThunder();
	virtual void Tick(float DeltaTime) override;
	
protected:
	UFUNCTION()
	void OnBoxComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                const FHitResult& SweepResult);
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ParticleSystemComponent;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxComponent;

	UPROPERTY()
	AActor* EnemyRef;

	UPROPERTY()
	APlayerCharacter* DamageCauser;

	float Damage = 0.f;
	float AttackInterval = 1.f;

	float DamageMultiplier = 1.f;

	FVector RandomLocation;

	FTimerHandle DurationTimerHandle;
	FTimerHandle IntervalTimerHandle;

	// Function
	void ResetThunder();

	

public:	
	void Initialize(APlayerCharacter* InDamageCauser, float InDamage, float InDamageMultiplier, float InAttackInterval);
	void RestartThunder();
	void ClearAllTimer();
};
