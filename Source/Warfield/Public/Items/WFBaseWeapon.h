// Warfield Game. All Rigths Reserved

#pragma once

#include "CoreMinimal.h"
#include "Items/WFBaseItem.h"
#include "WFCoreTypes.h"
#include "WFBaseWeapon.generated.h"

class AWFBaseCharacter;
class USoundCue;
/**
 *
 */
UCLASS()
class WARFIELD_API AWFBaseWeapon : public AWFBaseItem
{
    GENERATED_BODY()

public:
    FOnWeaponStateChangedSignature OnWeaponStateChanged;

    void StartFire();
    void StopFire();

    void ThrowWeapon();

    FORCEINLINE bool GetIsWeaponFalling() const { return bIsWeaponFalling; }

    FORCEINLINE EAmmoType GetWeaponAmmoType() const { return AmmoType; }
    FORCEINLINE int32 GetCurrentBulletAmount() const { return CurrentBulletAmount; }
    FORCEINLINE UTexture2D* GetAmmoTypeBulletTexture() const { return AmmoTypeBulletTexture; }
    FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
    FORCEINLINE FName GetReloadSectionName() const { return ReloadSectionName; }

    void Reload(int32& CharacterBulletAmount);

    bool IsAmmoEmpty() const;
    bool IsAmmoFull() const;

protected:
    virtual void BeginPlay() override;

private:
    /* Weapon Properties */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
    EWeaponType WeaponType{EWeaponType::EWT_Revolver};

    /* Set value relatively Fire Recoil Anim Montage */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
    float ShootTimeRate{0.6f};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
    float ShootTraceDistance = 20000.0f;

    /* Anim Montages*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
    FName FireSectionName{TEXT("Start Fire")};
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation", meta = (AllowPrivateAccess = "true"))
    FName ReloadSectionName{TEXT("Reload Revolver")};
    //

    FTimerHandle ShootTimerHandle;

    bool bIsButtonFirePressed{false};

    UPROPERTY()
    EWeaponState CurrentWeaponState{EWeaponState::EWS_Unoccupied};
    //

    /* Weapon VFX*/
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon VFX", meta = (AllowPrivateAccess = "true"))
    FName WeaponMuzzleFXSocketName = "MuzzleSocket";

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon VFX", meta = (AllowPrivateAccess = "true"))
    UParticleSystem* MuzzleFX;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon VFX", meta = (AllowPrivateAccess = "true"))
    USoundCue* ShootSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon VFX", meta = (AllowPrivateAccess = "true"))
    UParticleSystem* ImpactFX;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon VFX", meta = (AllowPrivateAccess = "true"))
    UParticleSystem* NoHitImpactFX;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon VFX", meta = (AllowPrivateAccess = "true"))
    UParticleSystem* TraceFX;
    //

    /* Ammo */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Ammo", meta = (AllowPrivateAccess = "true"))
    EAmmoType AmmoType{EAmmoType::EIR_Revolver};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Ammo", meta = (AllowPrivateAccess = "true"))
    UTexture2D* AmmoTypeBulletTexture;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Ammo", meta = (AllowPrivateAccess = "true"))
    int32 DefaultBulletAmount{30};

    int32 CurrentBulletAmount{0};

    //

    /* Throwing */
    FTimerHandle ThrowingTimerHandle;
    float WeaponFallingTime{0.6f};
    bool bIsWeaponFalling{false};

    void StartFireTimer();
    void ResetFireTimer();

    void MakeShot();

    void MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd, FVector& TraceFXEnd);

    void InitFX() const;
    void SpawnImpactFX(const FHitResult& HitResult, const FVector& TraceFXEnd) const;
    void SpawnTraceFX(const FVector& TraceFXStart, const FVector& TraceFXEnd) const;

    void PlayFireRecoilAnimMontage();

    void StopFalling();

    void SetAmmoData() const;
    void DecreaseAmmo();

    void SetWeaponState(EWeaponState NewWeaponState);

    // Getters
    FTransform GetMuzzleSocketTransform() const;
    AWFBaseCharacter* GetCharacter() const;
};
