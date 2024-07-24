// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Vampire2D/Enums/ProjectileState.h"
#include "Projectile.generated.h"

class UPaperFlipbook;
class UProjectileMovementComponent;
class UPaperFlipbookComponent;
class UCapsuleComponent;
class USphereComponent;

UCLASS()
class VAMPIRE2D_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();
	
	void InitializeProperties(const FVector& OverallScale, float Damage, const FVector& ProjectileVelc, FName TarTag,
		AActor* InDamageCauser, UPaperFlipbook* NewFlipbook, const FVector& FlipbookScale, float LifeTime, const FRotator& FlipbookRotation);

	void ResetStateByTimer();
	
	void SetProjectileState(const EProjectileState State) { ProjectileState = State; }
	EProjectileState GetProjectileState() const { return ProjectileState; }
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(VisibleAnywhere)
	UPaperFlipbookComponent* PaperFlipbookComponent;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;

	FName TargetTag;

	FVector ProjectileVelocity;

	float ProjectileDamage;

	UPROPERTY()
	AActor* DamageCauser;

	UPROPERTY()
	TArray<AActor*> DamagedActors;

	EProjectileState ProjectileState = EProjectileState::EPS_InPool;

public:	
	virtual void Tick(float DeltaTime) override;

};
