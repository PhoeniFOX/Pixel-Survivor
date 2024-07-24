#pragma once

UENUM(BlueprintType)
enum class ECharacterDirection : uint8
{
	ECD_Up UMETA(DisplayName = "Up"),
	EAS_Down UMETA(DisplayName = "Down"),
	EAS_Left UMETA(DisplayName = "Left"),
	EAS_Right UMETA(DisplayName = "Right")
};