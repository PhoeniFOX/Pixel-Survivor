#pragma once

UENUM(BlueprintType)
enum class EPickupType : uint8
{
	EPT_EXP1 UMETA(DisplayName = "EXP1"),
	EPT_EXP2 UMETA(DisplayName = "EXP2"),
	EPT_Magnet UMETA(DisplayName = "Magnet"),
	EPT_Max UMETA(DisplayName = "DefaultMax")
};