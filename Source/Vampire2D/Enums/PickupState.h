#pragma once

UENUM(BlueprintType)

enum class EPickupState : uint8
{
	EPS_Ready UMETA(DisplayName = "Ready"),
	EPS_Moving UMETA(DisplayName = "Moving"),
	EPS_InPool UMETA(DisplayName = "InPool"),
	EPS_Max UMETA(DisplayName = "DefaultMax")
};