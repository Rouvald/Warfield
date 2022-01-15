// Warfield Game. All Rigths Reserved

#include "Items/WFBaseWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

#include "WFBaseCharacter.h"
#include "Components/WFCrossHairComponent.h"
#include "WFUtils.h"

DEFINE_LOG_CATEGORY_STATIC(LogWFBaseWeapon, All, All)

void AWFBaseWeapon::StartFire()
{
    bIsButtonFirePressed = true;
    StartFireTimer();
}

void AWFBaseWeapon::StartFireTimer()
{
    if (!GetWorld()) return;
    if (!bCanFire) return;

    bCanFire = false;
    GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &AWFBaseWeapon::ResetFireTimer, ShootTimeRate);
    MakeShot();
}

void AWFBaseWeapon::ResetFireTimer()
{
    bCanFire = true;
    if (bIsButtonFirePressed)
    {
        StartFireTimer();
    }
}

void AWFBaseWeapon::StopFire()
{
    bIsButtonFirePressed = false;
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
    if(CurrentItemState == EItemState::EIS_Falling)
    {
        const FRotator MeshRotation{0.0f, ItemMesh->GetComponentRotation().Yaw, 1.0f};
        ItemMesh->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
    }
    OnItemStateChanged.Broadcast(EItemState::EIS_Pickup);
}

void AWFBaseWeapon::MakeShot()
{
    const auto Character = GetBaseCharacter();
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

    // DrawDebugSphere(GetWorld(), WeaponHitResult.ImpactPoint, 10.0f, 16, FColor::Green, false, 10.0f);

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

    // Shoot CrossHair Spread
    const auto CrossHairComponent = Character->FindComponentByClass<UWFCrossHairComponent>();
    if (CrossHairComponent)
    {
        CrossHairComponent->StartCrossHairShoot();
    }
}

void AWFBaseWeapon::MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd, FVector& TraceFXEnd)
{
    if (!GetWorld()) return;

    GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);
    // DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 10.0f);
    if (HitResult.bBlockingHit)
    {
        TraceFXEnd = HitResult.ImpactPoint;
        bIsHit = true;
    }
    else
    {
        bIsHit = false;
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
    const auto Character = GetBaseCharacter();
    if (!Character) return;
    // Muzzle FX
    /* @todo: In future remade with SpawnAtLocation */
    UGameplayStatics::SpawnEmitterAttached(MuzzleFX, //
        Character->GetMesh(),                        //
        WeaponMuzzleFXSocketName,                    //
        FVector::ZeroVector,                         //
        FRotator::ZeroRotator,                       //
        EAttachLocation::SnapToTarget,               //
        true                                         //
        );
    //UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFX, GetMuzzleSocketLocation(), GetMuzzleSocketLocation().Rotation());

    // Sound
    UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ShootSound, GetMuzzleSocketLocation());
}

void AWFBaseWeapon::SpawnImpactFX(const FHitResult& HitResult, const FVector& TraceFXEnd) const
{
    // Impact FX
    bIsHit
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

FVector AWFBaseWeapon::GetMuzzleSocketLocation() const
{
    const auto Character = GetBaseCharacter();
    if (!Character) return FVector::ZeroVector;

    return Character->GetMesh()->GetSocketLocation(WeaponMuzzleFXSocketName);
}

AWFBaseCharacter* AWFBaseWeapon::GetBaseCharacter() const
{
    return Cast<AWFBaseCharacter>(GetOwner());
}
