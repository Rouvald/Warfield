// Warfield Game. All Rigths Reserved

#pragma once

#include "CoreMinimal.h"
#include "WFCoreTypes.h"
#include "Components/ActorComponent.h"
#include "WFWeaponComponent.generated.h"

class AWFBaseCharacter;
class AWFBaseWeapon;
class AWFBaseItem;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class WARFIELD_API UWFWeaponComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UWFWeaponComponent();

    void StartFire();
    void StopFire();

    void TakeWeaponButtonPressed();
    void TakeWeaponButtonReleased();

    void DropWeaponButtonPressed();
    void DropWeaponButtonReleased();

    void ReloadButtonPressed();

    void GetPickupItem(AWFBaseItem* Item);

    FORCEINLINE AWFBaseWeapon* GetCurrentWeapon() const { return CurrentWeapon; }
    FORCEINLINE FWeaponData GetCurrentWeaponData() const { return CurrentWeaponData; }
    int32 GetDefaultWeaponAmmo() const;

protected:
    virtual void BeginPlay() override;

private:
    // Weapon
    UPROPERTY()
    AWFBaseWeapon* CurrentWeapon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon", meta=(AllowPrivateAccess="true"))
    TSubclassOf<AWFBaseWeapon> DefaultWeaponClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Ammo", meta = (AllowPrivateAccess = "true"))
    TMap<EWeaponType, FWeaponData> WeaponDataMap;

    FWeaponData CurrentWeaponData;
    //

    /* Ammo Map*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ammo", meta=(AllowPrivateAccess="true"))
    TMap<EAmmoType, int32> DefaultAmmoMap;
    //

    AWFBaseWeapon* SpawnWeapon() const;
    void AttachWeaponToComponent(AWFBaseWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName);

    void EquipWeapon(AWFBaseWeapon* EquippedWeapon);

    void SwapWeapon(AWFBaseWeapon* NewWeapon);
    void DropWeapon() const;

    void ReloadFinish();

    AWFBaseCharacter* GetCharacter() const;
};
