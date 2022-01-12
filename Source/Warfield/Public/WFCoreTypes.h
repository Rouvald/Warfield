// Warfield Game. All Rights Reserved

#pragma once

#include "WFCoreTypes.generated.h"

// ABaseItem

class AWFBaseItem;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemAreaOverlapSignature, const AWFBaseItem*, bool);
//DECLARE_MULTICAST_DELEGATE_OneParam(FOnTraceHitItemSignature, bool);

USTRUCT(BlueprintType)
struct FSomeStruct
{
    GENERATED_USTRUCT_BODY()
};
