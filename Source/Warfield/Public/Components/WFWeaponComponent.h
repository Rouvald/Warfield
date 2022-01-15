// Warfield Game. All Rigths Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WFWeaponComponent.generated.h"

class AWFBaseCharacter;
class AWFBaseWeapon;

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

protected:
    virtual void BeginPlay() override;

private:
    // Weapon
    UPROPERTY()
    AWFBaseWeapon* CurrentWeapon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon", meta=(AllowPrivateAccess="true"))
    TSubclassOf<AWFBaseWeapon> DefaultWeaponClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon",
        meta=(AllowPrivateAccess="true", ToolTip="Sockets: WeaponSocket_L and WeaponSocket_R"))
    FName WeaponSocketName = "WeaponSocket_L";
    //

    AWFBaseWeapon* SpawnWeapon() const;
    void AttachWeaponToComponent(AWFBaseWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName);

    void EquipWeapon(AWFBaseWeapon* EquippedWeapon);

    void SwapWeapon(AWFBaseWeapon* NewWeapon);
    void DropWeapon() const;

    AWFBaseCharacter* GetBaseCharacter() const;
};
