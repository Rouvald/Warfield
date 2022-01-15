// Warfield Game. All Rights Reserved

#pragma once

#include "WFCoreTypes.generated.h"

// ABaseItem

class AWFBaseItem;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemAreaOverlapSignature, const AWFBaseItem*, bool);
// DECLARE_MULTICAST_DELEGATE_OneParam(FOnTraceHitItemSignature, bool);

/* Weapon*/
UENUM(BlueprintType)
enum class EItemRarity : uint8
{
    EIR_Damaged UMETA(DisplayName="Damaged"),
    EIR_Uncommon UMETA(DisplayName="Uncommon"),
    EIR_Rare UMETA(DisplayName = "Rare"),
    EIR_Mythical UMETA(DisplayName = "Mythical"),
    EIR_Legendary UMETA(DisplayName = "Legendary"),
    EIR_Immortal UMETA(DisplayName = "Immortal"),

    EIR_MAX UMETA(DisplayName = "MAX")
};

UENUM(BlueprintType)
enum class EItemState : uint8
{
    EIS_Pickup,
    // UMETA(DisplayName="Pickup"),
    EIS_EquipInProgress,
    // UMETA(DisplayName="EquipInProgress"),
    EIS_PickedUp,
    // UMETA(DisplayName="PikcedUp"),
    EIS_Equipped,
    // UMETA(DisplayName="Equipped"),
    EIS_Falling,
    // UMETA(DisplayName="Falling"),

    EIS_MAX //UMETA(DisplayName = "MAX")
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemStateChangedSignature, EItemState);

enum ECollisionResponse;

USTRUCT(BlueprintType)
struct FItemProperties
{
    GENERATED_USTRUCT_BODY()

    // Item Mesh
    UPROPERTY()
    bool bIsSimulatedPhysics;

    UPROPERTY()
    bool bIsGravityEnable;

    UPROPERTY()
    bool bIsVisible;

    ECollisionResponse ItemMeshCollisionResponseToAllChannels;

    ECollisionChannel ItemMeshCollisionChannel;
    ECollisionResponse ItemMeshCollisionResponseToChannel;

    ECollisionEnabled::Type ItemMeshCollisionEnabled;
    //
    // Area Collision
    ECollisionResponse AreaCollisionResponseToAllChannels;

    ECollisionEnabled::Type AreaCollisionEnabled;
    //
    // Box Collision
    ECollisionResponse BoxCollisionResponseToAllChannels;

    ECollisionEnabled::Type BoxCollisionEnabled;

    ECollisionChannel BoxCollisionChannel;
    ECollisionResponse BoxCollisionResponseToChannel;
    //
};
