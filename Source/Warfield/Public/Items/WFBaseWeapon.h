// Warfield Game. All Rigths Reserved

#pragma once

#include "CoreMinimal.h"
#include "Items/WFBaseItem.h"
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
    void StartFire();
    void StopFire();

    void ThrowWeapon();

    FORCEINLINE bool GetIsWeaponFalling() const {return bIsWeaponFalling;}

private:
    /* Weapon Shooting*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    UAnimMontage* FireRecoilAnimMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    float ShootTimeRate = 0.6;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    float ShootTraceDistance = 20000.0f;

    FTimerHandle ShootTimerHandle;

    bool bIsButtonFirePressed{false};
    bool bCanFire{true};
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

    /* Throwing */
    FTimerHandle ThrowingTimerHandle;
    float WeaponFallingTime{0.6f};
    bool bIsWeaponFalling{false};

    void StartFireTimer();
    void ResetFireTimer();

    void MakeShot();

    void PlayFireRecoilAnimMon() const;

    void MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd, FVector& TraceFXEnd);

    void InitFX() const;
    void SpawnImpactFX(const FHitResult& HitResult, const FVector& TraceFXEnd) const;
    void SpawnTraceFX(const FVector& TraceFXStart, const FVector& TraceFXEnd) const;

    void StopFalling();
    // Getters
    FVector GetMuzzleSocketLocation() const;
    AWFBaseCharacter* GetBaseCharacter() const;
};
