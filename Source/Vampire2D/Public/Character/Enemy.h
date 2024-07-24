// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AITypes.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "Vampire2D/Enums/MobState.h"
#include "Vampire2D/Interfaces/CombatInterface.h"
#include "Enemy.generated.h"

struct FMonsterInfo;
class UParticleEmitter;
class UPaperFlipbook;
class AIndicator;
class UVampireGameInstance;
class UPaperFlipbookComponent;

UCLASS()
class VAMPIRE2D_API AEnemy : public ACharacter, public ICombatInterface
{
	GENERATED_BODY()

public:
	AEnemy();
	
protected:
	UFUNCTION()
	void OnCapsuleOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                           const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnCapsuleOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	virtual void BeginPlay() override;

	virtual void DisplayHealth(float inHealth) const;

	UFUNCTION(BlueprintCallable)
	void ReturnToPool();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Animations")
	UMaterialInstanceDynamic* TransparentMaterial;

	UFUNCTION(BlueprintCallable)
	void AfterFadeOut();
	

private:
	UPROPERTY(VisibleAnywhere)
	UPaperFlipbookComponent* PaperFlipbookComponent;
	
	bool bIsDead = false;

	FTimerHandle AttackTimerHandle;

	UPROPERTY()
	AActor* DamagedActor;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	UMaterialInstance* MaskedUnlitMaterial;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	UMaterialInstance* TranslucentUnlitMaterial;
	
	UPROPERTY(EditDefaultsOnly, Category= "UI")
	TSubclassOf<class AFloatingTextActor> FloatingTextActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
	UDataTable* MonsterDataTable;
	
	FTimerHandle ReturnToPoolTimerHandle;

	bool bToLeft = false;
	bool bCanMoveToTouch = true;
	bool bRandomLocation = false;
	FVector TeleportLocation;
	bool bMovetoPlayer = false;
	FVector DestinationLocation;
	FName MonsterID;

	FTimerHandle RestartApproachingTimerHandle;
	FTimerHandle RestartApproachLocTimerHandle;
	FTimerHandle CheckSpawnLocationTimerHandle;

	bool bApproachLocTimerSet = false;

	EMobState MobState = EMobState::EMS_MAX;
	bool bIsPooling = true;

	FTimeline FadingOutTimeline;
	FOnTimelineFloat TimelineProgress;
	FOnTimelineEvent TimelineFinished;
	
	UPROPERTY(EditAnywhere, Category = "Animations")
	UCurveFloat* FadingOutCurve;

	UPROPERTY()
	UVampireGameInstance* VampireGameInstance;

	FTimerHandle CheckDistanceTimerHandle;

	UPROPERTY()
	AIndicator* Indicator;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TSubclassOf<AIndicator> IndicatorClass;

	bool bBoss16Cooling = false;

	FRotator Boss16ShootDirection = FRotator::ZeroRotator;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	UPaperFlipbook* BossBulletFlipBook;

	bool bBoss31Cooling = false;

	UPROPERTY(EditDefaultsOnly, Category= "VFX" )
	UParticleSystem* Boss31Effect;

	UPROPERTY()
	UParticleSystemComponent* Boss31VFX;
	
	UPROPERTY(EditDefaultsOnly, Category= "VFX")
	UPaperFlipbook* Boss31SkillFlipBook;

	FMonsterInfo* MonsterInfo;

	UPROPERTY()
	AFloatingTextActor* FloatingTextActor = nullptr;

	// Enemy Info
	FText InfoName;
	float CurrentHealth;
	float InfoMass = 1.f;
	float InfoAttack = 10.f;
	float InfoHP = 10.f;
	float InfoExp = 10.f;
	float InfoTreasureDropRate = 0.1f;
	float InfoInitWalkSpeed = 100.f;

	// Functions
	void TouchDamage();
	void HandleDamageText(float InDamage);
	void StartMarching();
	void HandleDirection();
	void MoveToPlayer(bool bInbMovetoPlayer, const FVector& InDestinationLocation);
	void RestartApproaching();
	void RestartApproachingLocation();
	void PoolingRevive(bool bInMoveToPlayer, const FVector& InDestinationLocation);
	void CheckSpawnLocation();
	void PoolingDeath();
	void ResetStatusByTimer();
	void Boss16();
	void Boss16DelayShoot();
	void Boss31();

	UFUNCTION()
	void FadingOutTimelineProgress(float Value);

	void Boss31SkillStart();
	void Boss31SkillEnd();

	void Boss31Indicator();
	void Boss31DelayShoot();

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void IHitHandler_Implementation(AActor* InDamageCauser, float InDamage, int32 InForce) override;

	void ResetColorAndMovement();
	void HandleKnockBackAndColor(AActor* InDamageCauser, int32 InForce);

	void MoveToPlayerSuccess();
	void MoveToPlayerFail();
	void MoveToLocationSuccess();
	void MoveToLocationFail();
	void ResetMoving();
	void SetMobState(EMobState InMobState);
	EMobState GetMobState() const { return MobState; }
	void SetIsPooling(bool bInIsPooling) { bIsPooling = bInIsPooling; }
	void SpawnSetup(FName InMonsterID, bool bInbRandomLocation, const FVector& InTeleportLocation, bool bInbMovetoPlayer,
				const FVector& InDestinationLocation);

	void PoolingPickups();
	FORCEINLINE bool GetIsMoveToPlayer() const {return bMovetoPlayer;}
	void ForceDeath();
};
