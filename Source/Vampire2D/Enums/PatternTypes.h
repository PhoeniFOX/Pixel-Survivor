#pragma once

UENUM(BlueprintType)
enum class EPatternTypes : uint8
{
	EPT_CROSS UMETA(DisplayName = "Cross"),
	EPT_CIRCLE UMETA(DisplayName = "Circle"),
	EPT_GRID UMETA(DisplayName = "Grid"),
	EPT_MAX UMETA(DisplayName = "DefaultMAX")
};