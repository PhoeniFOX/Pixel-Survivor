#pragma once

UENUM(BlueprintType)
enum class ESoundType : uint8
{
	EST_HitSound UMETA(DisplayName = "HitSound"),
	EST_CoinSound UMETA(DisplayName = "CoinSound")
};