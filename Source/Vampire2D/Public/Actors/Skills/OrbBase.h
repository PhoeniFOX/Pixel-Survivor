// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrbBase.generated.h"

class APlayerCharacter;
class USphereComponent;
class UPaperFlipbookComponent;

UCLASS()
class VAMPIRE2D_API AOrbBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AOrbBase();
	virtual void Tick(float DeltaTime) override;
	
protected:
	UFUNCTION()
	void OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay() override;
	
private:
	UPROPERTY(VisibleAnywhere)
	UPaperFlipbookComponent* PaperFlipbookComponent;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	APlayerCharacter* DamageCauser;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	float Damage = 1.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	float DamageMultiplier = 1.f;

	UPROPERTY()
	AActor* EnemyRef;
public:	
	void Initialize(APlayerCharacter* InDamageCauser, float InDamage, float InDamageMultiplier);

};
