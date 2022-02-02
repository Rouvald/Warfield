// Warfield Game. All Rights Reserved

#include "Items/WFBaseWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

#include "WFBaseCharacter.h"
//#include "Components/WFCrossHairComponent.h"
#include "WFUtils.h"
#include "WFWeaponComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogWFBaseWeapon, All, All)

void AWFBaseWeapon::BeginPlay()
{
    Super::BeginPlay();

    checkf(MaximumBulletInMagazine > 0, TEXT("Maximum Bullet in Magazine can't be <= 0"));
    CurrentBulletAmount = MaximumBulletInMagazine;

    OnWeaponStateChanged.AddUObject(this, &AWFBaseWeapon::SetWeaponState);
}

void AWFBaseWeapon::StartFire()
{
    bIsButtonFirePressed = true;
    /* Wait still Aim animation is blending */
    GetWorldTimerManager().SetTimer(StartShootDelayTimerHandle, this, &AWFBaseWeapon::MakeShot, 0.1f);
}

void AWFBaseWeapon::StartFireTimer()
{
    if (!GetWorld()) return;
    SetWeaponState(EWeaponState::EWS_FireInProgress);
    GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &AWFBaseWeapon::ResetFireTimer, ShootTimeRate);
}

void AWFBaseWeapon::ResetFireTimer()
{
    if(CurrentWeaponState == EWeaponState::EWS_Reloading) return;
    
    SetWeaponState(EWeaponState::EWS_Unoccupied);
    if (bIsButtonFirePressed)
    {
        if (!IsAmmoEmpty())
        {
            MakeShot();
        }
    }
    if (IsAmmoEmpty())
    {
        AutoReload();
    }
}

void AWFBaseWeapon::StopFire()
{
    bIsButtonFirePressed = false;
}

bool AWFBaseWeapon::CanFire() const
{
    return CurrentWeaponState == EWeaponState::EWS_Unoccupied && !IsAmmoEmpty();
}

void AWFBaseWeapon::MakeShot()
{
    if (!GetWorld() || !CanFire()) return;

    const auto Character = GetCharacter();
    if (!Character) return;

    FVector TraceStart, TraceEnd;
    if (!WFUtils::GetTraceData(Character, TraceStart, TraceEnd, ShootTraceDistance)) return;

    InitFX();

    // Bullet Trace End FVector
    FVector TraceFXEnd{TraceEnd};
    //
    // Hit from ViewPoint
    FHitResult DefaultHitResult;
    MakeHit(DefaultHitResult, TraceStart, TraceEnd, TraceFXEnd);

    // Hit from WeaponMuzzleSocket
    FHitResult WeaponHitResult;
    FVector WeaponTraceStart{GetMuzzleSocketTransform().GetLocation()};
    /*
     * @note: Sometimes Trace can't reach DefaultHitResult.ImpactPoint
     *        This 2 lines fix this error
     */
    const FVector StartToEndTraces{TraceFXEnd - WeaponTraceStart};
    FVector WeaponTraceEnd{WeaponTraceStart + StartToEndTraces * 1.1f};
    //
    /* MakeHit for second trace, from weapon muzzle socket */
    MakeHit(WeaponHitResult, WeaponTraceStart, WeaponTraceEnd, TraceFXEnd);

    /*
     * @note We have 2 FHitResult: DefaultHitResult and WeaponHitResult.
     *       It's needed for correct build trace from WeaponMuzzle and HitPoint (if have HitPoint).
     *       For Correct working FX we need use WeaponHitResult. Because If DefaultHitResult hit someone,
     *       WeaponHitResult anyways hit same Object and will have same ImpactPoint.
     *
     *       Another situation:  Nothing had been hit
     *       In this case for spawning FX will be used TraceFXEnd.
     */

    // Spawn FX
    SpawnImpactFX(WeaponHitResult, TraceFXEnd);
    SpawnTraceFX(GetMuzzleSocketTransform().GetLocation(), TraceFXEnd);
    //

    // Decrease Ammo after make shot
    DecreaseAmmo();

    // Start timer for next Weapon shot
    StartFireTimer();

    // Play recoil animation
    PlayFireRecoilAnimMontage();

    // Shoot CrossHair Spread
    /*const auto CrossHairComponent = Character->FindComponentByClass<UWFCrossHairComponent>();
    if (CrossHairComponent)
    {
        CrossHairComponent->StartCrossHairShoot();
    }*/
}

void AWFBaseWeapon::MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd, FVector& TraceFXEnd)
{
    if (!GetWorld()) return;

    GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);
    // DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 10.0f);
    if (HitResult.bBlockingHit)
    {
        TraceFXEnd = HitResult.ImpactPoint;
    }
}


void AWFBaseWeapon::InitFX() const
{
    // Muzzle FX
    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFX, GetMuzzleSocketTransform());

    // Sound
    UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ShootSound, GetMuzzleSocketTransform().GetLocation());
}

void AWFBaseWeapon::SpawnImpactFX(const FHitResult& HitResult, const FVector& TraceFXEnd) const
{
    // Impact FX
    HitResult.bBlockingHit
        ? UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactFX, HitResult.ImpactPoint, HitResult.Normal.Rotation())
        : UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), NoHitImpactFX, TraceFXEnd);
}

void AWFBaseWeapon::SpawnTraceFX(const FVector& TraceFXStart, const FVector& TraceFXEnd) const
{
    // Trace FX
    const auto TraceFXComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TraceFX, TraceFXStart);
    if (TraceFXComponent)
    {
        TraceFXComponent->SetVectorParameter(FName("Target"), TraceFXEnd);
    }
}

void AWFBaseWeapon::PlayFireRecoilAnimMontage()
{
    const auto Character = GetCharacter();
    if (!Character) return;

    const auto WeaponComponent = Character->FindComponentByClass<UWFWeaponComponent>();
    if (!WeaponComponent) return;

    WFUtils::PlayAnimMontage(Character, WeaponComponent->GetCurrentWeaponData().FireRecoilAnimMontage, FireSectionName);
}

void AWFBaseWeapon::ThrowWeapon()
{
    const FRotator MeshRotation{0.0f, ItemMesh->GetComponentRotation().Yaw, 1.0f};
    ItemMesh->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);

    const auto MeshForwardVector{ItemMesh->GetForwardVector()};
    const auto MeshRightVector{ItemMesh->GetRightVector()};
    auto ThrowingDirection{MeshRightVector.RotateAngleAxis(-20.f, MeshForwardVector)};

    const float RandomRotation{FMath::FRandRange(25.0f, 35.0f)};
    ThrowingDirection = ThrowingDirection.RotateAngleAxis(RandomRotation, FVector{0.0f, 0.0f, 1.0f});
    ThrowingDirection *= 5000.0f;

    ItemMesh->AddImpulse(ThrowingDirection);

    bIsWeaponFalling = true;
    GetWorldTimerManager().SetTimer(ThrowingTimerHandle, this, &AWFBaseWeapon::StopFalling, WeaponFallingTime, false);
}

void AWFBaseWeapon::StopFalling()
{
    bIsWeaponFalling = false;
    if (CurrentItemState == EItemState::EIS_Falling)
    {
        const FRotator MeshRotation{0.0f, ItemMesh->GetComponentRotation().Yaw, 1.0f};
        ItemMesh->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
    }
    OnItemStateChanged.Broadcast(EItemState::EIS_Pickup);
}

void AWFBaseWeapon::Reload(int32 ReloadingAmmo)
{
    if (CurrentBulletAmount + ReloadingAmmo > MaximumBulletInMagazine)
    {
        UE_LOG(LogWFBaseWeapon, Error, TEXT("Bullet for reloading more maximum bullet in magazine"));
        return;
    }
    CurrentBulletAmount += ReloadingAmmo;
}

/* @todo: rework auto reload (maybe)*/
void AWFBaseWeapon::AutoReload() const
{
    const auto Character = GetCharacter();
    if (!Character) return;

    const auto WeaponComponent = Character->FindComponentByClass<UWFWeaponComponent>();
    if (!WeaponComponent) return;

    WeaponComponent->ReloadButtonPressed();
}

bool AWFBaseWeapon::CanReload() const
{
    return !IsAmmoFull() && CurrentWeaponState != EWeaponState::EWS_Reloading;
}

void AWFBaseWeapon::SetWeaponState(EWeaponState NewWeaponState)
{
    if (CurrentWeaponState == NewWeaponState) return;
    CurrentWeaponState = NewWeaponState;
    //UE_LOG(LogWFBaseWeapon, Display, TEXT("%s"), *UEnum::GetValueAsString(NewWeaponState));
}

void AWFBaseWeapon::SetAmmoData() const
{
}

bool AWFBaseWeapon::IsAmmoEmpty() const
{
    return CurrentBulletAmount == 0;
}

bool AWFBaseWeapon::IsAmmoFull() const
{
    return CurrentBulletAmount == MaximumBulletInMagazine;
}

void AWFBaseWeapon::DecreaseAmmo()
{
    if (IsAmmoEmpty()) return;

    --CurrentBulletAmount;

    if (IsAmmoEmpty())
    {
        StopFire();
    }
}

FTransform AWFBaseWeapon::GetMuzzleSocketTransform() const
{
    //return ItemMesh->GetSocketLocation(WeaponMuzzleFXSocketName);
    return ItemMesh->GetSocketTransform(WeaponMuzzleFXSocketName);
}

AWFBaseCharacter* AWFBaseWeapon::GetCharacter() const
{
    return Cast<AWFBaseCharacter>(GetOwner());
}
