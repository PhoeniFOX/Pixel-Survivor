// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HolyShrine.generated.h"

class APlayerCharacter;

UCLASS()
class VAMPIRE2D_API AHolyShrine : public AActor
{
	GENERATED_BODY()
	
public:	
	AHolyShrine();
	virtual void Tick(float DeltaTime) override;
protected:
	UFUNCTION()
	void OnStaticMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                              const FHitResult& SweepResult);
	UFUNCTION()
	void OnStaticMeshEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	virtual void BeginPlay() override;


private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* HSRoot;
	
	float AttackInterval = 1.f;

	UPROPERTY()
	APlayerCharacter* DamageCauser;
	
	UPROPERTY()
	TArray<AActor*> InAreaActors;
	
	float Damage = 0.f;
	float Dilation = 1.f;

	FTimerHandle AreaDmgTimerHandle;

	// Functions
	void AreaDamage();

public:	
	void Initialize(float InAttackInterval, float InDmg, float InDilation, APlayerCharacter* InDamageCauser);

};
