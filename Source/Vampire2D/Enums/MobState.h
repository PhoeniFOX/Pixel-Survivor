#pragma once

UENUM(BlueprintType)
enum class EMobState : uint8
{
	EMS_MoveToPlayer UMETA(DisplayName = "MoveToPlayer"),
	EMS_InPool UMETA(DisplayName = "InPool"),
	EMS_Initializing UMETA(DisplayName = "Initializing"),
	EMS_Special UMETA(DisplayName = "Special"),
	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};