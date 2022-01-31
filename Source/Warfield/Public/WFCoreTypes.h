// Warfield Game. All Rights Reserved

#pragma once

#include "WFCoreTypes.generated.h"

// ABaseItem

class AWFBaseItem;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemAreaOverlapSignature, const AWFBaseItem*, bool);
// DECLARE_MULTICAST_DELEGATE_OneParam(FOnTraceHitItemSignature, bool);

/* Item */
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

/* Weapon Component */

USTRUCT(BlueprintType)
struct FWeaponData
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon",
        meta=(AllowPrivateAccess="true", ToolTip="Sockets: WeaponSocket_L and WeaponSocket_R"))
    FName WeaponSocketName = "WeaponSocket_L";

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Ammo", meta = (AllowPrivateAccess = "true"))
    UAnimMontage* ReloadAnimMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Data", meta = (AllowPrivateAccess = "true"))
    UAnimMontage* FireRecoilAnimMontage;
};

/* Weapon */

// Weapon Type

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    EWT_Revolver UMETA(DisplayName="Revolver"),
    EWT_SubmachineGun UMETA(DisplayName="SubmachineGun"),
    EWT_AssaultRifle UMETA(DisplayName="AssaultRifle"),

    EWT_MAX UMETA(DisplayName = "MAX")
};

// Weapon Data

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
    EIR_Revolver UMETA(DisplayName="Revolver"),
    EIR_AR UMETA(DisplayName="Assault Rifle"),

    EIR_MAX UMETA(DisplayName = "MAX")
};


// Weapon State

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
    EWS_Unoccupied UMETA(DisplayName="Unoccupied"),
    EWS_FireInProgress UMETA(DisplayName="FireInProgress"),
    EWS_Reloading UMETA(DisplayName="Reloading"),

    EWS_MAX UMETA(DisplayName = "MAX")
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnWeaponStateChangedSignature, EWeaponState);
//

// Weapon Reload rotate chamber

/*USTRUCT(BlueprintType)
struct FWeaponChamberBones
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Skeleton", meta = (AllowPrivateAccess = "true"))
    TArray<FName> WeaponBladeWingBoneNames;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Skeleton", meta = (AllowPrivateAccess = "true"))
    FName WeaponBarrelBoneName{TEXT("gun_real_barrel")};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Skeleton", meta = (AllowPrivateAccess = "true"))
    FName WeaponChamberBoneName{TEXT("gun_chamber")};
};*/
//