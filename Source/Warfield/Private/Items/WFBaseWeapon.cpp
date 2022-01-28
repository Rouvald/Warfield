// Warfield Game. All Rights Reserved

#include "Items/WFBaseWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

#include "WFBaseCharacter.h"
//#include "Components/WFCrossHairComponent.h"
#include "WFUtils.h"

DEFINE_LOG_CATEGORY_STATIC(LogWFBaseWeapon, All, All)

void AWFBaseWeapon::BeginPlay()
{
    Super::BeginPlay();

    checkf(DefaultBulletAmount > 0, TEXT("Bullet amount can be <= 0"));
    CurrentBulletAmount = DefaultBulletAmount;
}

void AWFBaseWeapon::StartFire()
{
    bIsButtonFirePressed = true;
    MakeShot();
}

void AWFBaseWeapon::StartFireTimer()
{
    if (!GetWorld()) return;
    CurrentWeaponState = EWeaponState::EWS_FireInProgress;
    GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &AWFBaseWeapon::ResetFireTimer, ShootTimeRate);
}

void AWFBaseWeapon::ResetFireTimer()
{
    CurrentWeaponState = EWeaponState::EWS_Unoccupied;
    if (!IsAmmoEmpty())
    {
        if (bIsButtonFirePressed)
        {
            MakeShot();
        }
    }
}

void AWFBaseWeapon::StopFire()
{
    bIsButtonFirePressed = false;
}

void AWFBaseWeapon::MakeShot()
{
    if (!GetWorld() || CurrentWeaponState != EWeaponState::EWS_Unoccupied || IsAmmoEmpty())
    {
        StopFire();
        return;
    }

    const auto Character = GetBaseCharacter();
    if (!Character) return;

    FVector TraceStart, TraceEnd;
    if (!WFUtils::GetTraceData(Character, TraceStart, TraceEnd, ShootTraceDistance))
    {
        StopFire();
        return;
    }

    InitFX();

    // Bullet Trace End FVector
    FVector TraceFXEnd{TraceEnd};
    //
    // Hit from ViewPoint
    FHitResult DefaultHitResult;
    MakeHit(DefaultHitResult, TraceStart, TraceEnd, TraceFXEnd);

    // Hit from WeaponMuzzleSocket
    FHitResult WeaponHitResult;
    FVector WeaponTraceStart{GetMuzzleSocketLocation()};
    /*
     * @note: Sometimes Trace can't reach DefaultHitResult.ImpactPoint
     *        This 2 lines fix this error
     */
    const FVector StartToEndTraces{TraceFXEnd - GetMuzzleSocketLocation()};
    FVector WeaponTraceEnd{GetMuzzleSocketLocation() + StartToEndTraces * 1.1f};
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
    SpawnTraceFX(GetMuzzleSocketLocation(), TraceFXEnd);
    //

    // Play recoil animation
    PlayFireRecoilAnimMon();

    // Decrease Ammo after make shot
    DecreaseAmmo();

    // Start timer for next Weapon shot
    StartFireTimer();

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

void AWFBaseWeapon::PlayFireRecoilAnimMon() const
{
    const auto Character = GetBaseCharacter();
    if (!Character) return;

    const auto AnimInstance = Character->GetMesh()->GetAnimInstance();
    if (AnimInstance && FireRecoilAnimMontage)
    {
        AnimInstance->Montage_Play(FireRecoilAnimMontage);
        AnimInstance->Montage_JumpToSection(FName("StartFire")); // Name of Section in AnimMontage
    }
}

void AWFBaseWeapon::InitFX() const
{
    // Muzzle FX
    /*const auto Character = GetBaseCharacter();
    if (!Character) return;*/

    /* @todo: In future remade with SpawnAtLocation */
    /*UGameplayStatics::SpawnEmitterAttached(MuzzleFX, //
        Character->GetMesh(),                        //
        WeaponMuzzleFXSocketName,                    //
        FVector::ZeroVector,                         //
        FRotator::ZeroRotator,                       //
        EAttachLocation::SnapToTarget,               //
        true                                         //
        );*/
    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFX, GetMuzzleSocketLocation(), GetMuzzleSocketLocation().Rotation());

    // Sound
    UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ShootSound, GetMuzzleSocketLocation());
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

void AWFBaseWeapon::SetAmmoData() const
{
}

bool AWFBaseWeapon::IsAmmoEmpty() const
{
    return CurrentBulletAmount == 0;
}

bool AWFBaseWeapon::IsAmmoFull() const
{
    return CurrentBulletAmount == DefaultBulletAmount;
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

FVector AWFBaseWeapon::GetMuzzleSocketLocation() const
{
    return ItemMesh->GetSocketLocation(WeaponMuzzleFXSocketName);
}

AWFBaseCharacter* AWFBaseWeapon::GetBaseCharacter() const
{
    return Cast<AWFBaseCharacter>(GetOwner());
}
