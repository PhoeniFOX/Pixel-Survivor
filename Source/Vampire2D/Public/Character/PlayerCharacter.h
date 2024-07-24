// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "Vampire2D/Enums/CharacterState.h"
#include "Vampire2D/Interfaces/CombatInterface.h"
#include "Vampire2D/Interfaces/InteractionInterface.h"
#include "Vampire2D/Structs/FPlayerInfo.h"
#include "Vampire2D/Structs/FSkillInfo.h"
#include "PlayerCharacter.generated.h"

class USoundCue;
class UOptions;
struct FTimeline;
class ASkyThunder;
class AHolyShrine;
class AOrbContainer;
class USkillTab;
class USkillSelection;
class USphereComponent;
class AProjectile;
class UPaperFlipbook;
class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class UPaperFlipbookComponent;
class USpringArmComponent;

UCLASS()
class VAMPIRE2D_API APlayerCharacter : public ACharacter, public ICombatInterface, public IInteractionInterface
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void IHitHandler_Implementation(AActor* inDamageCauser, float inDamage, int32 inForce) override;
	virtual bool IGetIsDead_Implementation() const override;
	virtual void IAddEXP_Implementation(float inExpVal) override;
	virtual void ITriggerMagnet_Implementation() override;
	virtual void ITriggerKill_Implementation() override;
protected:
	virtual void BeginPlay() override;
private:
	/* Properties */
	UPROPERTY(VisibleAnywhere)
	UPaperFlipbookComponent* PaperFlipbookComponent;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* PickupSphere;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputAction* IA_Move;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputAction* IA_Special;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	UInputAction* IA_Options;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Properties")
	float Exp_Mul = 1.f;
	
	bool bIsDead = false;

	ECharacterDirection CharacterDirection;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TArray<UPaperFlipbook*> MoveAnimations;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TArray<UPaperFlipbook*> IdleAnimations;

	UPROPERTY()
	UPaperFlipbook* CurrentAnimation;

	int32 ProjectileVelocity = 200;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	UPaperFlipbook* FireBallFlipbook;

	bool bIsInvincible = false;

	UPROPERTY(EditDefaultsOnly, Category = "SFX")
	USoundCue* HitSoundCue;
	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	FPlayerInfo PlayerInfo;
	
	TArray<FSkillInfo> SkillPools;

	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
	UDataTable* CommonSkillDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
	UDataTable* UniqueSkillDataTable;
	
	float CurrentHealth = 0.f;
	float PickupSphereRadius = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class AFloatingTextActor> FloatingTextActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USkillSelection> SkillSelectionClass;

	UPROPERTY()
	class UVampireGameInstance* VampireGameInstance;
	
	int32 SkillLevel = 0;

	FTimerHandle PlayerAttackTimerHandle;

	int32 LevelUpDelay = 0;

	int32 KillCount = 0;

	bool bIsP_esEnabled = false;
	int32 p_es_Level = 1;

	bool bIsP_dcEnabled = false;
	int32 p_dc_Level = 1;

	float p_haste = 1.f;
	

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	TSubclassOf<AOrbContainer> OrbContainerClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	TSubclassOf<AOrbContainer> SwordContainerClass;
	
	UPROPERTY()
	AOrbContainer* OrbContainer;

	UPROPERTY()
	AOrbContainer* SwordContainer;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	TSubclassOf<AHolyShrine> HolyShrineClass;

	UPROPERTY()
	AHolyShrine* HolyShrine;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	TSubclassOf<ASkyThunder> SkyThunderClass;
	
	TArray<ASkyThunder*> SkyThunders;

	int32 c_br_SkillLevel = 0;

	FTimerHandle brTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	UPaperFlipbook* brFlipBook;

	int32 c_spiral_SkillLevel = 0;
	FTimerHandle spiralTimerHandle;
	
	float p_ho_mul = 1.f;
	float Defence_mul = 1.f;

	int32 SpecialInterval = 1;
	float SpecialValue = 0.f;
	FTimerHandle SpecialTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "SFX")
	USoundCue* SpecialSoundCue;

	FTimeline SpecialTextTimeline;
	FOnTimelineLinearColor SpecialTextTimelineProgress;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UCurveLinearColor* SpecialTextColorCurve;

	bool bIsSpecialRest = false;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> SpecialFlashWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UOptions> OptionsWidget;

	UPROPERTY()
	AFloatingTextActor* FloatingTextActor;

	float AtkMin = 1.f;
	float AtkMax = 1.f;
	float FireWandAtkInterval = 1.f;
	
	/* Functions */
	void Move(const FInputActionValue& InputActionValue);
	void Options();
	void StopMove();
	void SpecialSkill();
	void UpdateCharacterDirection(const FVector2D& Movement);
	void UpdateAnimation(bool bIsMoving, ECharacterDirection FaceDirection);
	void Start_priest_firewand();
	void PreInitialization();
	void HandleSkillInitialization();
	void InitPlayerAttributes();
	void PostInitialization();
	void ResetColorAndInvincibility();
	void HandleHitColorAndInvincible();
	void HandleDamageText(float inDamage);

	UFUNCTION()
	void SelectedSkill(const FSkillInfo& InSkillInfo);
	
	void UpgradeSkill(const FString& InSkillTag, int32 InLevel);
	void Start_priest();
	int32 GetRequiredEXP(int32 inCurrentLevel) const;
	void CreateSkillSelection(FSkillInfo* InSkillInfo);
	void UpdateSkillPoolsInfo(const FString& InSkillTag);
	void SpecialTimer(int32 InInterval);
	void OnRecoveringSpecial();

	UFUNCTION()
	void OnSpecialTextColorChanged(FLinearColor Value);
	void ResetSpecialBar();
	void ShopModifier();

	int32 RandomEventEffectValue(const FString& InTag) const;

public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool IsDead() const { return bIsDead; }

	int32 GetKillCount() const { return KillCount; }
	int32 GetCurLevel() const { return PlayerInfo.CurLevel; }
	int32 GetCrtMul() const { return PlayerInfo.CRT_mul; }
	int32 GetAtkMul() const { return PlayerInfo.ATK_mul; }
	bool AddSkillSelection();

	void InitGameLayout();

	// Active Skills
	void c_ro();
	void c_ro_Create(float InRadius, int32 InOrbNum, float InOrbScale, float InRotatingSpeed, float InAtkMultiplier, float InDamage);
	void c_ro_Destroy();
	
	void c_hs();
	void c_hs_Create(float InAttackInterval, float InDmg, float InDilation, const FVector& InSpawnTransformScale);

	void c_st();
	void c_st_Create(int32 InNums, float InAttackInterval, float InDmg, float InAttackMultiplier, const FVector& InSpawnTransformScale);
	void c_st_Destroy();

	void c_br();
	void c_br_Shoot();

	void c_spiral();
	void c_spiral_Shoot();
	void c_spiral_Create(float InRadius, int32 InOrbNum, float InOrbScale, float InRotatingSpeed, float InAtkMultiplier, float InDamage);

	// Passive Skills
	void p_es();
	float p_es_mul(const AActor* InEnemy) const;

	void p_dc();
	float p_dc_mul() const;
	
	void p_fp();

	void p_na();

	void p_crit();

	void p_sc();

	void p_ho();

	void p_exp();
};
