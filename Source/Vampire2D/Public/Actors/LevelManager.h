// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Vampire2D/Enums/PickupTypes.h"
#include "LevelManager.generated.h"

class USoundCue;
struct FShopItemInfo;
class AMobPatternBase;
enum class EPatternTypes : uint8;
class APickup;
class AEnemy;
class AProjectile;
class UPaperFlipbook;

UCLASS()
class VAMPIRE2D_API ALevelManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelManager();

	virtual void Tick(float DeltaTime) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Enemy")
	TSubclassOf<AEnemy> EnemyClass;

	UPROPERTY(EditAnywhere, Category = "Pickup")
	TSubclassOf<APickup> PickupClass;
	
	TArray<AProjectile*> ProjectilePool;
	TArray<AEnemy*> AllEnemyPool;
	TArray<AEnemy*> ActiveEnemyPool;
	TArray<int32> AllowEnemyIDs;
	TArray<APickup*> PickupPool;
	int32 AllowActiveMobNums = 3;

	FTimerHandle TryRespawnTimerHandle;
	FTimerHandle TimeCountTimerHandle;

	int32 PlayGameTimer = 0;
	
	int32 FinalScore = 0;

	UPROPERTY(EditDefaultsOnly, Category= "Random Skills")
	TArray<FString> RandomEvents =
		{
			"rss_randomatk",
			"rss_randomcritprobability",
			"rss_randompickup",
			"rss_randomspd",
			"rss_randomdef"
		};

	bool bIsRandomEventConfirmed = false;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	TSubclassOf<AMobPatternBase> CrossPatternClass;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	TSubclassOf<AMobPatternBase> CirclePatternClass;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	TSubclassOf<AMobPatternBase> GridPatternClass;

	UPROPERTY(EditDefaultsOnly, Category= "SFX")
	USoundCue* NormalBGM;

	UPROPERTY(EditDefaultsOnly, Category= "SFX")
	USoundCue* BossBGM;

	// Functions
	void KillAllMobs();
	void RandomEvent();
	void CalcAndInitRandomBuff(FShopItemInfo& LItemInfo);
	void SaveCoinData();

	// Timer Functions
	void CallTimerFunctions();
	void Sec1();
	void Sec2();
	void Sec10();
	void Sec35();
	void Sec45();
	void Sec50();
	void Sec60();
	void Sec65();
	void Sec85();
	void Sec95();
	void Sec101();
	void Sec120();
	void Sec125();
	void Sec140();
	void Sec180();
	void Sec200();
	void Sec220();
	void Sec240();
	void Sec260();
	void Sec280();
	void Sec300();
	void Sec315();
	void Sec340();
	void Sec360();
	void Sec370();
	void Sec390();
	void Sec425();
	void Sec455();
	void Sec460();
	void Sec480();
	void Sec500();
	void Sec505();
	void Sec520();
	void Sec540();
	void Sec580();
	void Sec620();
	void Sec650();
	void Sec690();
	void Sec720();
	void Sec750();
	void Sec790();
	void Sec820();
	void Sec840();
	void Sec865();
	void Sec900();
	void Sec950();
	void Sec955();
	void Sec960();
	void Sec1000();
	void Sec1030();
	void Sec1050();
	void Sec1090();
	void Sec1200();
	void Sec1205();
	void Sec1215();
	void Sec1500();
public:	
	void PoolingProjectiles(const FVector& Location, const FVector& OverallScale, float Damage, const FVector& ProjectileVelocity,
	FName TargetTag, AActor* DamageCauser, UPaperFlipbook* NewFlipbook, const FVector& FlipbookScale, float LifeTime, const FRotator& InFlipbookRotation = FRotator(0, -180, -90));

	void PoolingMobs(bool bInIsRandom, int32 InID, const FVector& InLocation, bool bInMoveToPlayer, const FVector& InDestination = FVector::ZeroVector);
	void ContinuousMobs(int32 InAllowActiveMobNums, const TArray<int32>& InAllowEnemyID);

	void TryRespawnMoreMobs();
	void RemoveEnemyFromActivePool(AEnemy* InEnemy);

	void PoolingPickupBase(EPickupType InPickupType, const FVector& InLocation, int32 InExpValue);

	void TimeCount();

	void ClearTimer();

	void CalculateFinalScore();

	void CalculateCoins(int32 InFinalScore);

	void MobPatterns(const EPatternTypes& InPattern, int32 InMaxLoop, int32 InSpecifiedID);
	FORCEINLINE TArray<AEnemy*> GetActiveEnemyPool() {return ActiveEnemyPool;}

	void MobBurst(const TArray<int32>& InAllowMobsID, int32 InRepeatedLoops);

	void UnpoolingMobs(int32 InID, const FVector& InScale) const;
};
