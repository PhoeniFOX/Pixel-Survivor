// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/PlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PaperFlipbookComponent.h"
#include "Actors/FloatingTextActor.h"
#include "Actors/LevelManager.h"
#include "Actors/Skills/HolyShrine.h"
#include "Actors/Skills/OrbBase.h"
#include "Actors/Skills/OrbContainer.h"
#include "Actors/Skills/SkyThunder.h"
#include "Camera/CameraComponent.h"
#include "Character/Enemy.h"
#include "Components/CapsuleComponent.h"
#include "Components/HorizontalBox.h"
#include "Components/Overlay.h"
#include "Components/ProgressBar.h"
#include "Components/SphereComponent.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "Data/GameInstanceFunctionLibrary.h"
#include "Data/Vampire2DPlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Data/VampireGameInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Runtime/Core/Tests/Containers/TestUtils.h"
#include "Sound/SoundCue.h"
#include "UI/GameLayout.h"
#include "UI/Options.h"
#include "UI/SkillSelection.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	PickupSphere->SetupAttachment(RootComponent);

	PaperFlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("PaperFlipbook"));
	PaperFlipbookComponent->SetupAttachment(RootComponent);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->SetProjectionMode(ECameraProjectionMode::Orthographic);

	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
}
// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = PlayerInfo.MaxHP;
	PreInitialization();
	PostInitialization();
}

void APlayerCharacter::PreInitialization()
{
	CharacterDirection = ECharacterDirection::EAS_Down;
	UpdateAnimation(false, CharacterDirection);
	VampireGameInstance = Cast<UVampireGameInstance>(GetGameInstance());

	// Setup EnhancedInput System
	if (AVampire2DPlayerController* PC = Cast<AVampire2DPlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			LocalPlayerSubsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
}

void APlayerCharacter::PostInitialization()
{
	if (VampireGameInstance)
	{
		VampireGameInstance->PlayerCharacter = this;
	}

	ShopModifier();
	
	InitPlayerAttributes();

	if (VampireGameInstance)
	{
		FTimerHandle DelayTimerHandle;
		GetWorldTimerManager().SetTimer(DelayTimerHandle, this, &APlayerCharacter::InitGameLayout, .2, false);
	}
	if(UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->RetrieveShopModifier("rss_special") == 1)
	{
		SpecialTimer(SpecialInterval);
	}
	
	HandleSkillInitialization();
}

void APlayerCharacter::HandleSkillInitialization()
{
	for (int32 i = 0; i < 20; i++)
	{
		if (FSkillInfo* SkillInfo = CommonSkillDataTable->FindRow<FSkillInfo>(FName(*FString::FromInt(i)), TEXT(""), false))
		{
			SkillPools.Add(*SkillInfo);
		}
	}

	for (int32 i = 0; i < 20; i++)
	{
		if (FSkillInfo* SkillInfo = UniqueSkillDataTable->FindRow<FSkillInfo>(FName(*FString::FromInt(i)), TEXT(""), false))
		{
			for (FString SkillTag : PlayerInfo.UniqueSkillTags)
			{
				if (SkillTag == SkillInfo->SkillTag)
				{
					SkillPools.Add(*SkillInfo);
				}
			}
		}
	}

	for (FSkillInfo SkillInfo : SkillPools)
	{
		if (SkillInfo.SkillTag == PlayerInfo.InitialSkillTag)
		{
			SelectedSkill(SkillInfo);
		}
	}
}

void APlayerCharacter::InitPlayerAttributes()
{
	CurrentHealth = PlayerInfo.MaxHP;
	GetCharacterMovement()->MaxWalkSpeed = 100.f * PlayerInfo.SPD_mul;
	PickupSphereRadius = PlayerInfo.PickupRadius;
	PickupSphere->SetSphereRadius(PickupSphereRadius);
}


void APlayerCharacter::IHitHandler_Implementation(AActor* inDamageCauser, float inDamage, int32 inForce)
{
	if (inDamageCauser && inDamageCauser->ActorHasTag(FName("enemy")) && !bIsDead && !bIsInvincible)
	{
		bIsInvincible = true;
		UGameplayStatics::SpawnSoundAtLocation(this, HitSoundCue, GetActorLocation());
		float FinalDamage = inDamage * p_ho_mul * Defence_mul;
		CurrentHealth = FMath::Clamp(0.f, CurrentHealth - FinalDamage, PlayerInfo.MaxHP);
		UGameInstanceFunctionLibrary::GetGameLayout(this)->UpdateHP(CurrentHealth, PlayerInfo.MaxHP);
		HandleDamageText(FinalDamage);
		if (CurrentHealth <= 0)
		{
			bIsDead = true;
			GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
			GetCapsuleComponent()->SetCollisionObjectType(ECC_WorldStatic);
			SetActorTickEnabled(false);
			GetCharacterMovement()->StopMovementImmediately();
			GetWorldTimerManager().ClearTimer(PlayerAttackTimerHandle);
			PlayerAttackTimerHandle.Invalidate();
			UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->LevelManager->ClearTimer();
			UGameInstanceFunctionLibrary::GetGameLayout(this)->GameOver(false);
		}
		
		HandleHitColorAndInvincible();
	}
}

bool APlayerCharacter::IGetIsDead_Implementation() const
{
	return bIsDead;
}

void APlayerCharacter::IAddEXP_Implementation(float inExpVal)
{
	int32 RequiredEXP = GetRequiredEXP(PlayerInfo.CurLevel);
	int32 TotalEXP = inExpVal * Exp_Mul + PlayerInfo.CurEXP;
	if (TotalEXP >= RequiredEXP)
	{
		// Level up
		PlayerInfo.MaxHP += 5.f;
		PlayerInfo.CurLevel++;
		PlayerInfo.CurEXP = TotalEXP - RequiredEXP;
		// Since player needs to select a skill to upgrade when leveling up, we need to delay few seconds for pausing purpose when leveling up multiple times in few frames.
		LevelUpDelay++;
		IAddEXP_Implementation(0.f);
		UGameInstanceFunctionLibrary::GetGameLayout(this)->UpdateHP(CurrentHealth, PlayerInfo.MaxHP);
		// Pause the game and pick a skill to upgrade, then LevelUpDelay--
		if (AddSkillSelection())
		{
			UE_LOG(LogTemp, Warning, TEXT("Skip Skill Selection"));
		}
		else
		{
			UGameplayStatics::SetGamePaused(GetWorld(), true);
		}
	}
	else
	{
		PlayerInfo.CurEXP = TotalEXP;
	}
	Cast<AVampire2DPlayerController>(VampireGameInstance->PlayerController)->GetGameLayout()->UpdateEXPAndLevel(PlayerInfo.CurEXP, RequiredEXP, PlayerInfo.CurLevel);
}

void APlayerCharacter::ITriggerMagnet_Implementation()
{
	PickupSphere->SetSphereRadius(1024.f);
	FTimerHandle MagnetTimer;
	GetWorldTimerManager().ClearTimer(MagnetTimer);
	GetWorldTimerManager().SetTimer(MagnetTimer, [&]{PickupSphere->SetSphereRadius(PickupSphereRadius);}, 5.f, false);
}

void APlayerCharacter::ITriggerKill_Implementation()
{
	KillCount++;
	UGameInstanceFunctionLibrary::GetGameLayout(this)->UpdateKills(KillCount);
}

void APlayerCharacter::ResetColorAndInvincibility()
{
	bIsInvincible = false;
	PaperFlipbookComponent->SetSpriteColor(FLinearColor(0.46, 0.46, 0.46, 1));
}

void APlayerCharacter::HandleHitColorAndInvincible()
{
	PaperFlipbookComponent->SetSpriteColor(FLinearColor::Red);
	FTimerHandle InvincibleTimer;
	GetWorldTimerManager().SetTimer(InvincibleTimer, this, &APlayerCharacter::ResetColorAndInvincibility, 1.f, false);
}

void APlayerCharacter::HandleDamageText(float inDamage)
{
	if(FloatingTextActorClass)
	{
		FVector SpawnLocation = PaperFlipbookComponent->GetComponentLocation();
		if (FloatingTextActor == nullptr)
		{
			FloatingTextActor = GetWorld()->SpawnActor<AFloatingTextActor>(FloatingTextActorClass, SpawnLocation, FRotator::ZeroRotator);
			FloatingTextActor->AttachToComponent(PaperFlipbookComponent, FAttachmentTransformRules::KeepWorldTransform);
		}
		FloatingTextActor->DisplayFloatingText(FText::FromString(FString::FromInt(FMath::TruncToInt(inDamage))), FLinearColor::Red);
	}
}

void APlayerCharacter::SelectedSkill(const FSkillInfo& InSkillInfo)
{
	// Select a skill to upgrade
	UGameplayStatics::SetGamePaused(GetWorld(), false);
	UpgradeSkill(InSkillInfo.SkillTag, InSkillInfo.SkillLevel + 1);  // SkillLevel + 1 for skill switch base on Skilllevel (start from 1);
	UpdateSkillPoolsInfo(InSkillInfo.SkillTag);
	UGameInstanceFunctionLibrary::GetGameLayout(this)->DisplaySkillIconOnSlot(InSkillInfo);
	UGameInstanceFunctionLibrary::GetGameLayout(this)->WS_InGame->SetActiveWidgetIndex(0);
}

void APlayerCharacter::UpgradeSkill(const FString& InSkillTag, const int32 InLevel)
{
	SkillLevel = InLevel;
	if (InSkillTag == "start_priest")
	{
		FTimerHandle SkillUpgradeTimer;
		GetWorldTimerManager().SetTimer(SkillUpgradeTimer, this, &APlayerCharacter::Start_priest, .025f, false);
	}
	if (InSkillTag == "c_ro")
	{
		c_ro();
	}
	if (InSkillTag == "c_hs")
	{
		c_hs();
	}
	if (InSkillTag == "c_st")
	{
		c_st();
	}
	if (InSkillTag == "c_br")
	{
		c_br();
	}
	if (InSkillTag == "c_spiral")
	{
		c_spiral();
	}
	if (InSkillTag == "p_es")
	{
		p_es();
	}
	if (InSkillTag == "p_dc")
	{
		p_dc();
	}
	if (InSkillTag == "p_fp")
    {
    	p_fp();
    }
	if (InSkillTag == "p_na")
	{
		p_na();
	}
	if (InSkillTag == "p_crit")
	{
		p_crit();
	}
	if (InSkillTag == "p_sc")
	{
		p_sc();
	}
	if (InSkillTag == "p_ho")
	{
		p_ho();
	}
	if (InSkillTag == "p_exp")
	{
		p_exp();
	}
}

void APlayerCharacter::Start_priest()
{
	GetWorldTimerManager().ClearTimer(PlayerAttackTimerHandle);
	// Enforce current skill according to new level
	if (SkillLevel - 1 > 6) return;
	switch (SkillLevel - 1)
	{
		case 0:
			AtkMin = 25.f;
			AtkMax = 34.f;
			ProjectileVelocity = 200;
			FireWandAtkInterval = 1.f;
			break;

		case 1:
			AtkMin = 32.f;
			AtkMax = 45.f;
			ProjectileVelocity = 200;
			FireWandAtkInterval = 1.f;
			break;
			
		case 2:
			AtkMin = 40.f;
			AtkMax = 52.f;
			ProjectileVelocity = 200;
			FireWandAtkInterval = .8f;
			break;

		case 3:
			AtkMin = 40.f;
			AtkMax = 52.f;
			ProjectileVelocity = 300;
			FireWandAtkInterval = .7f;
			break;

		case 4:
			AtkMin = 45.f;
			AtkMax = 60.f;
			ProjectileVelocity = 300;
			FireWandAtkInterval = .6f;
			break;
			
		case 5:
			AtkMin = 45.f;
			AtkMax = 60.f;
			ProjectileVelocity = 300;
			FireWandAtkInterval = .5f;
			break;

		case 6:
			AtkMin = 50.f;
			AtkMax = 65.f;
		ProjectileVelocity = 350;
			FireWandAtkInterval = .4f;
			break;
		default: break;
	}
	GetWorldTimerManager().SetTimer(PlayerAttackTimerHandle, this, &APlayerCharacter::Start_priest_firewand, FireWandAtkInterval * p_haste, true);
}

int32 APlayerCharacter::GetRequiredEXP(int32 inCurrentLevel) const
{
	int32 CurLvRequireExp = pow((inCurrentLevel * 4),2);
	int32 NextLvRequireExp = pow((inCurrentLevel + 1) * 4, 2);
	return NextLvRequireExp - CurLvRequireExp;
}

bool APlayerCharacter::AddSkillSelection()
{
	UGameInstanceFunctionLibrary::GetGameLayout(this)->WS_InGame->SetActiveWidgetIndex(1);
	UGameInstanceFunctionLibrary::GetGameLayout(this)->VB_SkillList->ClearChildren();
	Test::Shuffle(SkillPools);
	int32 SkillNumsInSelection = 0;
	for (FSkillInfo& SkillInfo : SkillPools)
	{
		if (SkillInfo.SkillLevel != SkillInfo.SkillMaxLevel && SkillNumsInSelection != 4)
		{
			bool bActiveSkillMax = UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->GetActiveSlotNum() ==
				UGameInstanceFunctionLibrary::GetGameLayout(this)->HB_ActiveSkills->GetChildrenCount();
			bool bPassiveSkillMax = UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->GetActiveSlotNum() ==
				UGameInstanceFunctionLibrary::GetGameLayout(this)->HB_PassiveSkills->GetChildrenCount();

			if ((SkillInfo.bIsItem && !bPassiveSkillMax) || (!SkillInfo.bIsItem && !bActiveSkillMax) || !SkillInfo.bIsNew)
			{
				SkillNumsInSelection++;
				CreateSkillSelection(&SkillInfo);
			}
			else
			{
				FString SkillType = SkillInfo.bIsItem ? TEXT("passive") : TEXT("active");
				UE_LOG(LogTemp, Warning, TEXT("New %s Skill, %s reached max"), *SkillType, *SkillType);
			}
		}
	}

	if (SkillNumsInSelection == 0)
	{
		UGameInstanceFunctionLibrary::GetGameLayout(this)->WS_InGame->SetActiveWidgetIndex(0);
		return true;
	}
	return false;
}

void APlayerCharacter::InitGameLayout()
{
	if (AVampire2DPlayerController* VPlayerController = Cast<AVampire2DPlayerController>(VampireGameInstance->PlayerController))
	{
		VPlayerController->GetGameLayout()->UpdateHP(CurrentHealth, PlayerInfo.MaxHP);
		VPlayerController->GetGameLayout()->UpdateEXPAndLevel(PlayerInfo.CurEXP, GetRequiredEXP(PlayerInfo.CurLevel), PlayerInfo.CurLevel);
		VPlayerController->GetGameLayout()->UpdateKills(0);
	} 
}


void APlayerCharacter::CreateSkillSelection(FSkillInfo* InSkillInfo)
{
	if (USkillSelection* SkillSelection = CreateWidget<USkillSelection>(GetWorld(), SkillSelectionClass))
	{
		SkillSelection->SetSkillInfo(*InSkillInfo);
		UGameInstanceFunctionLibrary::GetGameLayout(this)->VB_SkillList->AddChild(SkillSelection);
		SkillSelection->SetPadding(FMargin(0.f, 10.f, 0.f, 10.f));
		SkillSelection->OnSkillSelected.AddDynamic(this, &APlayerCharacter::SelectedSkill);
	}
}

void APlayerCharacter::UpdateSkillPoolsInfo(const FString& InSkillTag)
{
	for (FSkillInfo& SkillInfo : SkillPools)
	{
		if (SkillInfo.SkillTag == InSkillTag)
		{
			SkillInfo.SkillLevel++;
			SkillInfo.bIsNew = false;
		}
	}
}

void APlayerCharacter::SpecialTimer(int32 InInterval)
{
	GetWorldTimerManager().SetTimer(SpecialTimerHandle, this, &APlayerCharacter::OnRecoveringSpecial, InInterval, true);
	SpecialValue++;
}

void APlayerCharacter::OnRecoveringSpecial()
{
	SpecialValue++;
	UGameInstanceFunctionLibrary::GetGameLayout(this)->UpdateSpecialTimer(SpecialValue);
	if (SpecialValue >= 10.f)
	{
		GetWorldTimerManager().PauseTimer(SpecialTimerHandle);
		UGameplayStatics::SpawnSound2D(this, SpecialSoundCue);
		if (SpecialTextColorCurve && !SpecialTextTimeline.IsPlaying())
		{
			bIsSpecialRest = false;
			SpecialTextTimelineProgress.BindUFunction(this, FName("OnSpecialTextColorChanged"));
			SpecialTextTimeline.AddInterpLinearColor(SpecialTextColorCurve, SpecialTextTimelineProgress);
			SpecialTextTimeline.SetLooping(true);
			SpecialTextTimeline.PlayFromStart();
		}
	}
	else
	{
		if (!bIsSpecialRest)
		{
			SpecialTextTimeline.Stop();
			UGameInstanceFunctionLibrary::GetGameLayout(this)->P_Special->SetFillColorAndOpacity(FLinearColor::Yellow);
			bIsSpecialRest = true;
		}
	}
}

void APlayerCharacter::OnSpecialTextColorChanged(FLinearColor Value)
{
	UGameInstanceFunctionLibrary::GetGameLayout(this)->P_Special->SetFillColorAndOpacity(Value);
}

void APlayerCharacter::ResetSpecialBar()
{
	bIsSpecialRest = false;
	SpecialTextTimeline.Stop();
	UGameInstanceFunctionLibrary::GetGameLayout(this)->P_Special->SetFillColorAndOpacity(FLinearColor::Yellow);
}

void APlayerCharacter::ShopModifier()
{
	UVampireGameInstance* GameInstance = UGameInstanceFunctionLibrary::GetVampireGameInstance(this);
	PlayerInfo.MaxHP += GameInstance->RetrieveShopModifier("rss_maxhp") * 5;
	
	PlayerInfo.ATK_mul += GameInstance->RetrieveShopModifier("rss_atk") * 0.1 +
		RandomEventEffectValue("rss_randomatk") * .03;
	
	PlayerInfo.CRT_mul += GameInstance->RetrieveShopModifier("rss_critprobability") * .02 +
		RandomEventEffectValue("rss_randomcritprobability") * .02;
	
	PlayerInfo.PickupRadius = (PlayerInfo.PickupRadius + GameInstance->RetrieveShopModifier("rss_pickup") * 7) *
	(RandomEventEffectValue("rss_randompickup") * .03 + 1);
	
	PlayerInfo.SPD_mul += PlayerInfo.SPD_mul + GameInstance->RetrieveShopModifier("rss_spd") * .05 +
	RandomEventEffectValue("rss_randomspd") * .02;

	Defence_mul = (1 - GameInstance->RetrieveShopModifier("rss_def") * .05) *
		(1 - RandomEventEffectValue("rss_randomdef") * .02);

	if (GameInstance->RetrieveShopModifier("rss_special") == 1)
	{
		UGameInstanceFunctionLibrary::GetGameLayout(this)->O_Special->SetVisibility(ESlateVisibility::Visible);
		switch (GameInstance->RetrieveShopModifier("rss_specialcd"))
		{
			case 0:
				SpecialInterval = 2.55f;
				break;
			case 1:
				SpecialInterval = 2.5f;
				break;
			case 2:
				SpecialInterval = 2.45f;
				break;
			case 3:
				SpecialInterval = 2.4f;
				break;
			case 4:
				SpecialInterval = 2.35f;
				break;
			case 5:
				SpecialInterval = 2.3f;
				break;
			case 6:
				SpecialInterval = 2.2f;
				break;
			case 7:
				SpecialInterval = 2.0f;
				break;
			case 8:
				SpecialInterval = 1.5f;
				break;
			default: break;
		}
	}
	else
	{
		UGameInstanceFunctionLibrary::GetGameLayout(this)->O_Special->SetVisibility(ESlateVisibility::Collapsed);
	}
}

int32 APlayerCharacter::RandomEventEffectValue(const FString& InTag) const
{
	if (UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->RandomEffectTag == InTag)
	{
		return UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->RetrieveShopModifier(InTag);
	}
	return 0;
}

void APlayerCharacter::c_ro()
{
	if (SkillLevel - 1 > 6) return;
	c_ro_Destroy();
	float Radius = 0.f;
	int32 OrbNum = 0;
	float OrbScale = 1.7f;
	float RotationSpeed = 0.f;
	float AtkMul = 1.f;
	float Damage = 0.f;
	
	switch (SkillLevel - 1)
	{
		case 0:
			Radius = 50.f;
			OrbNum = 3;
			RotationSpeed = -1.0f;
			Damage = 3.f;
			break;
		case 1:
			Radius = 55.f;
			OrbNum = 4;
			RotationSpeed = -1.0f;
			Damage = 3.f;
			break;
		case 2:
			Radius = 60.f;
			OrbNum = 4;
			RotationSpeed = -1.5f;
			Damage = 4.f;
			break;
		case 3:
			Radius = 65.f;
			OrbNum = 5;
			RotationSpeed = -1.5f;
			Damage = 4.f;
			break;
		case 4:
			Radius = 70.f;
			OrbNum = 5;
			RotationSpeed = -2.f;
			Damage = 6.f;
			break;
		case 5:
			Radius = 75.f;
			OrbNum = 6;
			RotationSpeed = -2.f;
			Damage = 6.f;
			break;
		case 6:
			Radius = 80.f;
			OrbNum = 6;
			RotationSpeed = -2.5f;
			Damage = 9.f;
			break;
		default: break;
	}
	RotationSpeed = RotationSpeed * (2 - p_haste);
	c_ro_Create(Radius, OrbNum, OrbScale, RotationSpeed, AtkMul, Damage);
}

void APlayerCharacter::c_ro_Create(float InRadius, int32 InOrbNum, float InOrbScale, float InRotatingSpeed,
	float InAtkMultiplier, float InDamage)
{
	OrbContainer = GetWorld()->SpawnActor<AOrbContainer>(OrbContainerClass, GetActorLocation(), FRotator::ZeroRotator);
	OrbContainer->SetupRotationSpeed(InRotatingSpeed);
	OrbContainer->CreateOrbs(InRadius, InOrbNum, InOrbScale, this, InAtkMultiplier, InDamage, 0);
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
	OrbContainer->AttachToComponent(GetCapsuleComponent(), AttachmentRules);
}

void APlayerCharacter::c_ro_Destroy()
{
	if (OrbContainer)
	{
		for (AOrbBase* Orb : OrbContainer->GetOrbArray())
		{
			Orb->Destroy();
		}
		OrbContainer->Destroy();
	}
}

void APlayerCharacter::p_es()
{
	if (SkillLevel - 1 > 2) return;
	p_es_Level = SkillLevel - 1;
	bIsP_esEnabled = true;
}

float APlayerCharacter::p_es_mul(const AActor* InEnemy) const
{
	if(!bIsP_esEnabled) return 1.f;
	if (InEnemy)
	{
		// Calculate the distance between player and enemy see if its less than 1m
		if (GetDistanceTo(InEnemy) > 100.f)
		{
			switch (p_es_Level)
			{
				case 1: return 1.1f;
				case 2: return 1.2f;
				case 3: return 1.3f;
				default: return 1.f;
			}
		}
		switch (p_es_Level)
		{
			case 1: return 0.9f;
			case 2: return 0.85f;
			case 3: return 0.8f;
			default: return 1.f;
		}
	}
	return 1.f;
}

void APlayerCharacter::p_dc()
{
	if (SkillLevel - 1 > 2) return;
	p_dc_Level = SkillLevel - 1;
	bIsP_dcEnabled = true;
}

float APlayerCharacter::p_dc_mul() const
{
	if(!bIsP_dcEnabled) return 1.f;
	if(CurrentHealth / PlayerInfo.MaxHP >= 0.5f) return .5f;
	switch (p_dc_Level)
	{
		case 1: return 1.2f;
		case 2: return 1.4f;
		case 3: return 1.6f;
		default: return 1.f;
	}
}

void APlayerCharacter::p_fp()
{
	if (SkillLevel - 1 > 2) return;
	switch (SkillLevel - 1)
	{
		case 0:
			p_haste = 1.f - .1f;
			break;

		case 1:
			p_haste = 1.f - .2f;
			break;

		case 2:
			p_haste = 1.f - .3f;
			break;

		default: break;
	}
}

void APlayerCharacter::p_na()
{
	if (SkillLevel - 1 > 2) return;
	float AttackMul = 1.f;
	float CriticalMul = 1.f;
	switch (SkillLevel - 1)
	{
	case 0:
		AttackMul = 1.2f;
		CriticalMul = .8f;
		break;

	case 1:
		AttackMul = 1.3f;
		CriticalMul = .4f;
		break;

	case 2:
		AttackMul = 1.5f;
		CriticalMul = .1f;
		break;

	default: break;
	}
	PlayerInfo.ATK_mul = PlayerInfo.ATK_mul * AttackMul;
	PlayerInfo.CRT_mul = PlayerInfo.CRT_mul * CriticalMul;
}

void APlayerCharacter::p_crit()
{
	if (SkillLevel - 1 > 4) return;
	float CriticalMul = 1.f;
	switch (SkillLevel - 1)
	{
	case 0:
		CriticalMul = 1.5f;
		break;

	case 1:
		CriticalMul = 2.f;
		break;

	case 2:
		CriticalMul = 2.5f;
		break;

	case 3:
		CriticalMul = 3.f;
		break;

	case 4:
		CriticalMul = 4.f;
		break;
	
	default: break;
	}
	PlayerInfo.CRT_mul = PlayerInfo.CRT_mul * CriticalMul;
}

void APlayerCharacter::p_sc()
{
	if (SkillLevel - 1 > 2) return;
	float PickupMul = 1.f;
	switch (SkillLevel - 1)
	{
	case 0:
		PickupMul = 1.2f;
		break;

	case 1:
		PickupMul = 1.6f;
		break;

	case 2:
		PickupMul = 1.8f;
		break;

	default: break;
	}
	PickupSphereRadius *= PickupMul;
	PickupSphere->SetSphereRadius(PickupSphereRadius);
}

void APlayerCharacter::p_ho()
{
	if (SkillLevel - 1 > 2) return;
	float SpeedMul = 1.f;
	switch (SkillLevel - 1)
	{
	case 0:
		SpeedMul = 1.1f;
		p_ho_mul = 1.25f;
		break;

	case 1:
		SpeedMul = 1.2f;
		p_ho_mul = 1.2f;
		break;

	case 2:
		SpeedMul = 1.3f;
		p_ho_mul = 1.15f;
		break;

	default: break;
	}
	float WalkSpeed = PlayerInfo.SPD_mul * SpeedMul * 100.f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void APlayerCharacter::p_exp()
{
	if (SkillLevel - 1 > 2) return;
	switch (SkillLevel - 1)
	{
	case 0:
		Exp_Mul = 1.1f;
		break;

	case 1:
		Exp_Mul = 1.2f;
		break;

	case 2:
		Exp_Mul = 1.3f;
		break;

	default: break;
	}
}

void APlayerCharacter::c_hs()
{
	if (SkillLevel - 1 > 6) return;
	if (HolyShrine)
	{
		HolyShrine->Destroy();
	}
	float AtkSpeed = 1.f;
	float Damage = 0.f;
	float Scale = 1.f;
	float Dilation = 1.f;
	switch (SkillLevel - 1)
	{
		case 0:
			AtkSpeed = .65f;
			Damage = 4.f;
			Scale = 1.3f;
			Dilation = .85f;
			break;
		case 1:
			AtkSpeed = .65f;
			Damage = 4.f;
			Scale = 1.5f;
			Dilation = .85f;
			break;
		case 2:
			AtkSpeed = .65f;
			Damage = 4.6f;
			Scale = 1.5f;
			Dilation = .8f;
			break;
		case 3:
			AtkSpeed = .65f;
			Damage = 6.f;
			Scale = 1.5f;
			Dilation = .8f;
			break;
		case 4:
			AtkSpeed = .4f;
			Damage = 6.f;
			Scale = 1.8f;
			Dilation = .8f;
			break;
		case 5:
			AtkSpeed = .4f;
			Damage = 9.f;
			Scale = 1.8f;
			Dilation = .75f;
			break;
		case 6:
			AtkSpeed = .4f;
			Damage = 9.f;
			Scale = 1.8f;
			Dilation = .75f;
			break;
		default: break;
	}
	float AtkInterval = AtkSpeed * p_haste;
	FVector SpawnScale = FVector(Scale, Scale, 1.f);
	c_hs_Create(AtkInterval, Damage, Dilation, SpawnScale);
}

void APlayerCharacter::c_hs_Create(float InAttackInterval, float InDmg, float InDilation,
                                   const FVector& InSpawnTransformScale)
{
	FVector SpawnLocation = GetActorLocation() + FVector(0.f,0.f,-3.f);
	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	HolyShrine = GetWorld()->SpawnActor<AHolyShrine>(HolyShrineClass, SpawnLocation, FRotator::ZeroRotator, ActorSpawnParameters);
	HolyShrine->Initialize(InAttackInterval, InDmg, InDilation, this);
	HolyShrine->SetActorScale3D(InSpawnTransformScale);
	HolyShrine->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::KeepWorldTransform);
}

void APlayerCharacter::c_st()
{
	if (SkillLevel - 1 > 6) return;
	c_st_Destroy();

	int32 Nums = 1;
	float Damage = 50.f;
	float Scale = .8f;
	float AtkMultiplier = 1.f;
	float AttackInterval = 2.f;
	switch (SkillLevel - 1)
	{
	case 0:
		Damage = 50.f;
		AttackInterval = 2.f;
		break;
	case 1:
		Damage = 50.f;
		Scale = 1.2f;
		AttackInterval = 1.5;
		break;
	case 2:
		Damage = 150.f;
		Scale = 1.2f;
		AttackInterval = 1.5;
		break;
	case 3:
		Nums = 2;
		Damage = 150.f;
		Scale = 1.4f;
		AttackInterval = 1.5;
		break;
	case 4:
		Nums = 2;
		Damage = 300.f;
		Scale = 1.4f;
		AttackInterval = 1.f;
		break;
	case 5:
		Nums = 2;
		Damage = 350.f;
		Scale = 1.6f;
		AttackInterval = 1.f;
		break;
	case 6:
		Nums = 3;
		Damage = 400.f;
		Scale = 1.6f;
		AttackInterval = .6;
		break;
	default: break;
	}
	AttackInterval *= p_haste;
	FVector SpawnScale = FVector(Scale, Scale, Scale);
	c_st_Create(Nums, AttackInterval, Damage, AtkMultiplier, SpawnScale);
}

void APlayerCharacter::c_st_Create(int32 InNums, float InAttackInterval, float InDmg, float InAttackMultiplier,
	const FVector& InSpawnTransformScale)
{
	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	for (int32 i = 0; i < InNums; i++)
	{
		ASkyThunder* SkyThunder = GetWorld()->SpawnActor<ASkyThunder>(SkyThunderClass, GetActorLocation(), FRotator::ZeroRotator, ActorSpawnParameters);
		SkyThunder->Initialize(this, InDmg, InAttackMultiplier, InAttackInterval);
		SkyThunder->SetActorScale3D(InSpawnTransformScale);
		SkyThunders.Add(SkyThunder);
		SkyThunder->RestartThunder();
	}
}

void APlayerCharacter::c_st_Destroy()
{
	if (SkyThunders.Num() == 0) return;
	for (ASkyThunder* SkyThunder : SkyThunders)
	{
		SkyThunder->ClearAllTimer();
		SkyThunder->Destroy();
	}
	SkyThunders.Empty();
}

void APlayerCharacter::c_br()
{
	if (SkillLevel - 1 > 6) return;
	GetWorldTimerManager().ClearTimer(brTimerHandle);
	c_br_SkillLevel = SkillLevel - 1;
	float SkillInterval = 1.f;
	
	switch (c_br_SkillLevel)
	{
		case 0:
			SkillInterval = 2.f;
			break;

		case 1:
			SkillInterval = 2.f;
			break;

		case 2:
			SkillInterval = 1.f;
			break;

		case 3:
			SkillInterval = 1.f;
			break;
			
		case 4:
			SkillInterval = .6f;
			break;

		case 5:
			SkillInterval = .6f;
			break;

		case 6:
			SkillInterval = .6f;
			break;
			
		default: break;
	}
	SkillInterval *= p_haste;
	GetWorldTimerManager().SetTimer(brTimerHandle, this, &APlayerCharacter::c_br_Shoot, SkillInterval, true);
	c_br_Shoot();
}

void APlayerCharacter::c_br_Shoot()
{
	int32 Nums = 1;
	float SpawnScale = 1.f;
	float Damage = 1.f;
	FVector Direction = FVector::ZeroVector;
	switch (c_br_SkillLevel)
	{
		case 0:
			Nums = 1;
			SpawnScale = 1.f;
		Damage = 3.f;
			break;

		case 1:
			Nums = 2;
			SpawnScale = 1.f;
		
			break;

		case 2:
			Nums = 2;
			SpawnScale = 1.f;
		Damage = 6.f;
			break;

		case 3:
			Nums = 3;
			SpawnScale = 1.2f;
		Damage = 6.f;
			break;
				
		case 4:
			Nums = 3;
			SpawnScale = 1.2f;
		Damage = 6.f;
			break;

		case 5:
			Nums = 4;
			SpawnScale = 1.2f;
		Damage = 15.f;
			break;

		case 6:
			Nums = 6;
			SpawnScale = 1.4f;
		Damage = 15.f;
			break;
		default: break;
	}
	for (int i = 0; i < Nums; i++)
	{
		switch (i)
		{
		case 0:
			Direction = FVector(0.f, 150.f, 0.f);
			break;

		case 1:
			Direction = FVector(0.f, -150.f, 0.f);
			break;

		case 2:
			Direction = FVector(-150.f, 150.f, 0.f);
			break;

		case 3:
			Direction = FVector(150.f, -150.f, 0.f);
			break;
				
		case 4:
			Direction = FVector(150.f, 150.f, 0.f);
			break;

		case 5:
			Direction = FVector(-150.f, -150.f, 0.f);
			break;
		
			default: break;
		}
		UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->LevelManager->PoolingProjectiles(
			GetActorLocation(),
			FVector(SpawnScale),
			Damage,
			Direction,
			FName("enemy"),
			this,
			brFlipBook,
			FVector(SpawnScale),
			3,
			FRotator(0,-180,-90)
			);
	}
}

void APlayerCharacter::c_spiral()
{
	if (SkillLevel - 1 > 6) return;
	GetWorldTimerManager().ClearTimer(spiralTimerHandle);
	c_spiral_SkillLevel = SkillLevel - 1;
	float SkillInterval = 1.f;
	
	switch (c_spiral_SkillLevel)
	{
	case 0:
		SkillInterval = 2.f;
		break;

	case 1:
		SkillInterval = 1.6f;
		break;

	case 2:
		SkillInterval = 1.6f;
		break;

	case 3:
		SkillInterval = 1.6f;
		break;
			
	case 4:
		SkillInterval = 1.6f;
		break;

	case 5:
		SkillInterval = 1.6f;
		break;

	case 6:
		SkillInterval = 1.6f;
		break;
			
	default: break;
	}
	SkillInterval *= p_haste;
	GetWorldTimerManager().SetTimer(spiralTimerHandle, this, &APlayerCharacter::c_spiral_Shoot, SkillInterval, true);
	c_spiral_Shoot();
}

void APlayerCharacter::c_spiral_Shoot()
{
	float Radius = 10.f;
	int32 Nums = 1;
	float SpawnScale = 1.f;
	float RotationSpeed = -2.5;
	float AtkMultiplier = 1.f;
	float Damage = 1.f;
	switch (c_spiral_SkillLevel)
	{
		case 0:
			SpawnScale = 2.f;
		Damage = 5.f;
			break;
			
		case 1:
			SpawnScale = 2.f;
		Damage = 5.f;
			break;
        			
		case 2:
			SpawnScale = 3.f;
		Damage = 20.f;
			break;

		case 3:
			SpawnScale = 3.f;
		Damage = 20.f;
			break;

		case 4:
			SpawnScale = 4.f;
		Damage = 20.f;
			break;

		case 5:
			SpawnScale = 5.f;
		Damage = 20.f;
			break;

		case 6:
			SpawnScale = 5.f;
		Damage = 40.f;
			break;
		default: break;
	}
	c_spiral_Create(Radius, Nums, SpawnScale, RotationSpeed, AtkMultiplier, Damage);
}

void APlayerCharacter::c_spiral_Create(float InRadius, int32 InOrbNum, float InOrbScale, float InRotatingSpeed,
                                       float InAtkMultiplier, float InDamage)
{
	SwordContainer = GetWorld()->SpawnActor<AOrbContainer>(SwordContainerClass, GetActorLocation(), FRotator::ZeroRotator);
	SwordContainer->CreateSwords(InRadius, InOrbNum, InOrbScale, this, InAtkMultiplier, InDamage, 0);
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
	SwordContainer->AttachToComponent(GetCapsuleComponent(), AttachmentRules);
}



void APlayerCharacter::Move(const FInputActionValue& InputActionValue)
{
	if (IsDead()) return;
	FVector2D Movement = InputActionValue.Get<FVector2D>();
	AddMovementInput(FVector(1.f, 0.f, 0.f), Movement.X);
	AddMovementInput(FVector(0.f, 1.f, 0.f), Movement.Y);
	UpdateCharacterDirection(Movement);
	UpdateAnimation(true, CharacterDirection);
}

void APlayerCharacter::Options()
{
	UOptions* Widget = CreateWidget<UOptions>(GetWorld(), OptionsWidget);
	if (Widget)
	{
		Widget->AddToViewport();
	}
}

void APlayerCharacter::StopMove()
{
	UpdateAnimation(false, CharacterDirection);
}

void APlayerCharacter::SpecialSkill()
{
	if (SpecialValue < 10.f) return;
	SpecialValue = 0.f;
	UGameInstanceFunctionLibrary::GetGameLayout(this)->UpdateSpecialTimer(SpecialValue);
	UUserWidget* FlashWidget = CreateWidget<UUserWidget>(GetWorld(), SpecialFlashWidget);
	FlashWidget->AddToViewport();
	TArray<AEnemy*> Enemies = UGameInstanceFunctionLibrary::GetVampireGameInstance(this)->LevelManager->GetActiveEnemyPool();
	for (AEnemy* Enemy : Enemies)
	{
		if (UGameInstanceFunctionLibrary::CheckWithinScreen(this, Enemy->GetActorLocation()))
		{
			// TBD Multiplied with a value
			ICombatInterface::Execute_IHitHandler(Enemy, this, 1000.f * 1, 0);
		}
	}
	GetWorldTimerManager().UnPauseTimer(SpecialTimerHandle);
}


void APlayerCharacter::UpdateCharacterDirection(const FVector2D& Movement)
{
	if (Movement.X > 0.f)
	{
		CharacterDirection = ECharacterDirection::EAS_Right;
	}
	else if (Movement.X < 0.f)
	{
		CharacterDirection = ECharacterDirection::EAS_Left;
	}
	else if (Movement.Y > 0.f)
	{
		CharacterDirection = ECharacterDirection::EAS_Down;
	}
	else if (Movement.Y < 0.f)
	{
		CharacterDirection = ECharacterDirection::ECD_Up;
	}
}

void APlayerCharacter::UpdateAnimation(bool bIsMoving, ECharacterDirection FaceDirection)
{
	if (bIsMoving)
	{
		if (CurrentAnimation == nullptr || CurrentAnimation != MoveAnimations[static_cast<uint8>(CharacterDirection)])
		{
			CurrentAnimation = MoveAnimations[static_cast<uint8>(CharacterDirection)];
			PaperFlipbookComponent->SetFlipbook(CurrentAnimation);
		}
	}
	else
	{
		if (CurrentAnimation == nullptr || CurrentAnimation != IdleAnimations[static_cast<uint8>(CharacterDirection)])
		{
			CurrentAnimation = IdleAnimations[static_cast<uint8>(CharacterDirection)];
			PaperFlipbookComponent->SetFlipbook(CurrentAnimation);
		}
	}
}

void APlayerCharacter::Start_priest_firewand()
{
	FVector SpawnLocation = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z - 2.f);
	
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	FVector WorldLocation;
	FVector WorldDirection;
	PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

	FVector ProjectileDirection = FRotator(0,UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), WorldLocation).Yaw, 0).Vector();

	int32 Damage = FMath::RandRange(AtkMin, AtkMax);
	
	if (VampireGameInstance)
	{
		VampireGameInstance->LevelManager->PoolingProjectiles(
			SpawnLocation,
			FVector(1.f, 1.f, 1.f),
			Damage,
			ProjectileDirection * ProjectileVelocity,
			"enemy",
			this,
			FireBallFlipbook,
			FVector(1.f, 1.f, 1.f),
			3.f,
			FRotator(0, 90, -90)
		);
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SpecialTextTimeline.TickTimeline(DeltaTime);
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if(UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Completed, this, &APlayerCharacter::StopMove);
		EnhancedInputComponent->BindAction(IA_Special, ETriggerEvent::Started, this, &APlayerCharacter::SpecialSkill);
		EnhancedInputComponent->BindAction(IA_Options, ETriggerEvent::Started, this, &APlayerCharacter::Options);
	}
}
