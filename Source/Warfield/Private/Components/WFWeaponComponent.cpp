// Warfield Game. All Rigths Reserved

#include "Components/WFWeaponComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

#include "WFBaseCharacter.h"
#include "Components/WFCrossHairComponent.h"
#include "WFBaseWeapon.h"
#include "WFUtils.h"

DEFINE_LOG_CATEGORY_STATIC(LogWFWeaponComponent, All, All)

UWFWeaponComponent::UWFWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UWFWeaponComponent::BeginPlay()
{
    Super::BeginPlay();

    SpawnWeapon();
}

void UWFWeaponComponent::SpawnWeapon()
{
    const auto Character = GetBaseCharacter();
    if (!GetWorld() || !Character || !DefaultWeaponClass) return;

    const auto DefaultWeapon = GetWorld()->SpawnActor<AWFBaseWeapon>(DefaultWeaponClass);
    if (!DefaultWeapon) return;

    AttachWeaponToComponent(DefaultWeapon, Character->GetMesh(), WeaponSocketName);
}

void UWFWeaponComponent::AttachWeaponToComponent(AWFBaseWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName)
{
    if (!Weapon || !SceneComponent) return;

    const FAttachmentTransformRules AttachmentTransformRules{EAttachmentRule::SnapToTarget, false};
    Weapon->AttachToComponent(SceneComponent, AttachmentTransformRules, SocketName);
}

void UWFWeaponComponent::StartFire()
{
    bIsButtonFirePressed = true;
    StartFireTimer();
}

void UWFWeaponComponent::StartFireTimer()
{
    if (!GetWorld()) return;
    if (!bCanFire) return;

    bCanFire = false;
    GetWorld()->GetTimerManager().SetTimer(ShootTimerHandle, this, &UWFWeaponComponent::ResetFireTimer, ShootTimeRate);
    MakeShot();
}

void UWFWeaponComponent::ResetFireTimer()
{
    bCanFire = true;
    if (bIsButtonFirePressed)
    {
        StartFireTimer();
    }
}

void UWFWeaponComponent::StopFire()
{
    bIsButtonFirePressed = false;
}

void UWFWeaponComponent::MakeShot()
{
    if (!GetWorld()) return;

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

void UWFWeaponComponent::MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd, FVector& TraceFXEnd)
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

void UWFWeaponComponent::PlayFireRecoilAnimMon() const
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

void UWFWeaponComponent::InitFX() const
{
    const auto Character = GetBaseCharacter();
    if(!Character) return;
    // Muzzle FX
    /* TODO: In future remade with SpawnAtLocation*/
    UGameplayStatics::SpawnEmitterAttached(MuzzleFX, //
        Character->GetMesh(),                                   //
        WeaponMuzzleFXSocketName,                    //
        FVector::ZeroVector,                         //
        FRotator::ZeroRotator,                       //
        EAttachLocation::SnapToTarget,               //
        true                                         //
        );
    //UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFX, GetMuzzleSocketLocation());

    // Sound
    UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ShootSound, GetMuzzleSocketLocation());
}

void UWFWeaponComponent::SpawnImpactFX(const FHitResult& HitResult, const FVector& TraceFXEnd) const
{
    // Impact FX
    bIsHit
        ? UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactFX, HitResult.ImpactPoint, HitResult.Normal.Rotation())
        : UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), NoHitImpactFX, TraceFXEnd);
}

void UWFWeaponComponent::SpawnTraceFX(const FVector& TraceFXStart, const FVector& TraceFXEnd) const
{
    // Trace FX
    const auto TraceFXComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TraceFX, TraceFXStart);
    if (TraceFXComponent)
    {
        TraceFXComponent->SetVectorParameter(FName("Target"), TraceFXEnd);
    }
}

FVector UWFWeaponComponent::GetMuzzleSocketLocation() const
{
    const auto Character = GetBaseCharacter();
    if(!Character) return FVector::ZeroVector;
    
    return Character->GetMesh()->GetSocketLocation(WeaponMuzzleFXSocketName);
}

AWFBaseCharacter* UWFWeaponComponent::GetBaseCharacter() const
{
    return Cast<AWFBaseCharacter>(GetOwner());
}
