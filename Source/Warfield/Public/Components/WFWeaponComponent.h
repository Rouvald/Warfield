// Warfield Game. All Rigths Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WFWeaponComponent.generated.h"

class AWFBaseCharacter;
class AWFBaseWeapon;
class USoundCue;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class WARFIELD_API UWFWeaponComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UWFWeaponComponent();

    void StartFire();
    void StopFire();

protected:
    virtual void BeginPlay() override;

private:
    // Weapon
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    UAnimMontage* FireRecoilAnimMontage;

    UPROPERTY()
    AWFBaseWeapon* CurrentWeapon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon", meta=(AllowPrivateAccess="true"))
    TSubclassOf<AWFBaseWeapon> DefaultWeaponClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon",
        meta=(AllowPrivateAccess="true", ToolTip="Sockets: WeaponSocket_L and WeaponSocket_R"))
    FName WeaponSocketName = "WeaponSocket_L";
    //

    /* Weapon Shooting*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    float ShootTraceDistance = 20000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    float ShootTimeRate = 0.6;

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

    bool bIsHit = false;
    
    void StartFireTimer();
    void ResetFireTimer();

    void MakeShot();

    void PlayFireRecoilAnimMon() const;

    void MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd, FVector& TraceFXEnd);

    void InitFX() const;
    void SpawnImpactFX(const FHitResult& HitResult, const FVector& TraceFXEnd) const;
    void SpawnTraceFX(const FVector& TraceFXStart, const FVector& TraceFXEnd) const;

    FVector GetMuzzleSocketLocation() const;

    void SpawnWeapon();
    void AttachWeaponToComponent(AWFBaseWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName);

    AWFBaseCharacter* GetBaseCharacter() const;
};
