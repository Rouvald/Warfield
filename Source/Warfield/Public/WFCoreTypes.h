// Warfield Game. All Rights Reserved

#pragma once

#include "WFCoreTypes.generated.h"

// ABaseItem

class AWFBaseItem;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemAreaOverlapSignature, const AWFBaseItem*, bool);
// DECLARE_MULTICAST_DELEGATE_OneParam(FOnTraceHitItemSignature, bool);

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
    EIR_Damaged UMETA(DisplayName="Damaged"),
    EIR_Rare UMETA(DisplayName = "Rare"),
    EIR_Mythical UMETA(DisplayName = "Mythical"),
    EIR_Legendary UMETA(DisplayName = "Legendary"),
    EIR_Immortal UMETA(DisplayName = "Immortal"),

    EIR_MAX UMETA(DisplayName = "MAX")
};