#pragma once

UENUM(BlueprintType)
enum class EProjectileState : uint8
{
	EPS_InUse UMETA(DisplayName = "InUse"),
	EPS_InPool UMETA(DisplayName = "InPool")
};