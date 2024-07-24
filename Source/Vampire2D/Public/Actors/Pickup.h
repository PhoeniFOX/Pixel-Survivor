// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Vampire2D/Enums/PickupState.h"
#include "Vampire2D/Enums/PickupTypes.h"
#include "Pickup.generated.h"

class UPaperSprite;
struct FTimeline;
class UCapsuleComponent;
class UPaperSpriteComponent;
class USphereComponent;

UCLASS()
class VAMPIRE2D_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	APickup();
	
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void MagnetMovementFunction();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	UCapsuleComponent* CapsuleComponent;
	
	UPROPERTY(VisibleAnywhere)
	UPaperSpriteComponent* PaperSprite;

	UPROPERTY(EditDefaultsOnly, Category = "Pickup Properties")
	EPickupType PickupType = EPickupType::EPT_EXP1;
	
	EPickupState PickupState = EPickupState::EPS_Ready;

	float ExpValue = 0;

	int32 LoopingCount = 0;

	FTimeline MagnetTimeline;
	FTimeline EmptyTimeline = FTimeline(); 

	UPROPERTY()
	AActor* PlayerActor;

	UPROPERTY(EditDefaultsOnly, Category = "Pickup Properties")
	UPaperSprite* EXP01Sprite;

	UPROPERTY(EditDefaultsOnly, Category = "Pickup Properties")
	UPaperSprite* EXP02Sprite;

	UPROPERTY(EditDefaultsOnly, Category = "Pickup Properties")
	UPaperSprite* MagnetSprite;

	UPROPERTY()
	class UVampireGameInstance* VampireGameInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Pickup Properties")
	USoundBase* MagnetSound;

	// Function
	void AvoidFlicking();
	void ReturnToPool();
	void ResetLoopingCountAndCollision();
	void ItemEffect(AActor* TargetActor) const;

public:	
	void InitializePickup(EPickupType inPickupType, int32 inExpValue);

	EPickupState GetPickupState() const { return PickupState; }
};
