// Warfield Game. All Rigths Reserved

#include "Components/WFWeaponComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

#include "WFBaseCharacter.h"
#include "WFBaseWeapon.h"
#include "WFUtils.h"
#include "Components/AudioComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogWFWeaponComponent, All, All)

UWFWeaponComponent::UWFWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UWFWeaponComponent::BeginPlay()
{
    Super::BeginPlay();

    EquipWeapon(SpawnWeapon());
}

void UWFWeaponComponent::StartFire()
{
    if (!CurrentWeapon) return;
    CurrentWeapon->StartFire();
}

void UWFWeaponComponent::StopFire()
{
    if (!CurrentWeapon) return;
    CurrentWeapon->StopFire();
}

void UWFWeaponComponent::TakeWeaponButtonPressed()
{
    const auto Character = GetCharacter();
    if (!Character || !Character->GetHitItem()) return;

    const auto Weapon = Cast<AWFBaseWeapon>(Character->GetHitItem());
    if (Weapon)
    {
        Weapon->StartItemInterping(Character);
    }

    if(Character->GetHitItem()->GetPickupSound())
    {
        UGameplayStatics::SpawnSoundAtLocation(GetWorld(), Character->GetHitItem()->GetPickupSound(), Character->GetHitItem()->GetActorLocation());
    }
}

void UWFWeaponComponent::TakeWeaponButtonReleased()
{
}

void UWFWeaponComponent::DropWeaponButtonPressed()
{
    //DropItem();
}

void UWFWeaponComponent::DropWeaponButtonReleased()
{
}

void UWFWeaponComponent::ReloadButtonPressed()
{
    if (!CurrentWeapon || !CurrentWeapon->CanReload()) return;
    if (GetDefaultWeaponAmmo() <= 0) return;

    CurrentWeapon->StopFire();
    CurrentWeapon->OnWeaponStateChanged.Broadcast(EWeaponState::EWS_Reloading);

    const auto Character = GetCharacter();
    WFUtils::PlayAnimMontage(Character, CurrentWeaponData.ReloadAnimMontage, CurrentWeapon->GetReloadSectionName());
}

int32 UWFWeaponComponent::TakeAmmoForReload()
{
    if (!CurrentWeapon || CurrentWeapon->IsAmmoFull()) return 0;

    const auto CurrentWeaponAmmoType = CurrentWeapon->GetWeaponAmmoType();
    if (DefaultAmmoMap.Contains(CurrentWeaponAmmoType))
    {
        int32 NeededAmmo = CurrentWeapon->GetMaximumBulletInMagazine() - CurrentWeapon->GetCurrentBulletAmount();
        if (NeededAmmo <= CurrentWeapon->GetMaximumBulletInMagazine() && NeededAmmo > 0)
        {
            if (NeededAmmo > DefaultAmmoMap[CurrentWeaponAmmoType])
            {
                NeededAmmo = DefaultAmmoMap[CurrentWeaponAmmoType];
                DefaultAmmoMap[CurrentWeaponAmmoType] = 0;
                return NeededAmmo;
            }
            DefaultAmmoMap[CurrentWeaponAmmoType] -= NeededAmmo;
            return NeededAmmo;
        }
    }
    return 0;
}

void UWFWeaponComponent::ReloadFinish()
{
    if (!CurrentWeapon) return;

    const int32 NeededAmmo{TakeAmmoForReload()};
    if (NeededAmmo == 0) return;

    CurrentWeapon->Reload(NeededAmmo);

    CurrentWeapon->OnWeaponStateChanged.Broadcast(EWeaponState::EWS_Unoccupied);
}

void UWFWeaponComponent::StartRotateChamber()
{
    if (!CurrentWeapon || !CurrentWeapon->GetReloadSound()) return;

    if (!ReloadAudioComponent)
    {
        ReloadAudioComponent = UGameplayStatics::SpawnSoundAttached(CurrentWeapon->GetReloadSound(), CurrentWeapon->GetItemMesh(),
            CurrentWeapon->GetReloadBarrelName());
    }
    else
    {
        ReloadAudioComponent->SetPaused(false);
    }
}

void UWFWeaponComponent::StopRotateChamber()
{
    if(ReloadAudioComponent)
    {
        ReloadAudioComponent->SetPaused(true);
    }
}

int32 UWFWeaponComponent::GetDefaultWeaponAmmo() const
{
    if (!CurrentWeapon) return 0;

    if (!DefaultAmmoMap.Contains(CurrentWeapon->GetWeaponAmmoType())) return 0;

    return DefaultAmmoMap[CurrentWeapon->GetWeaponAmmoType()];
}

AWFBaseWeapon* UWFWeaponComponent::SpawnWeapon() const
{
    const auto Character = GetCharacter();
    if (!GetWorld() || !Character || !DefaultWeaponClass) return nullptr;

    const auto DefaultWeapon = GetWorld()->SpawnActor<AWFBaseWeapon>(DefaultWeaponClass);
    return DefaultWeapon;
}

void UWFWeaponComponent::EquipWeapon(AWFBaseWeapon* EquippedWeapon)
{
    if (!EquippedWeapon) return;

    const auto Character = GetCharacter();
    if (!Character) return;

    EquippedWeapon->SetOwner(Character);
    if (WeaponDataMap.Contains(EquippedWeapon->GetWeaponType()))
    {
        CurrentWeaponData = WeaponDataMap[EquippedWeapon->GetWeaponType()];
    }

    AttachWeaponToComponent(EquippedWeapon, Character->GetMesh(), CurrentWeaponData.WeaponSocketName);

    CurrentWeapon = EquippedWeapon;
    CurrentWeapon->OnItemStateChanged.Broadcast(EItemState::EIS_Equipped);
}

void UWFWeaponComponent::AttachWeaponToComponent(AWFBaseWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName)
{
    if (!Weapon || !SceneComponent) return;

    const FAttachmentTransformRules AttachmentTransformRules{EAttachmentRule::SnapToTarget, false};
    Weapon->AttachToComponent(SceneComponent, AttachmentTransformRules, SocketName);
}

void UWFWeaponComponent::SwapWeapon(AWFBaseWeapon* NewWeapon)
{
    const auto Character = GetCharacter();
    if (!Character) return;

    DropItem();
    EquipWeapon(NewWeapon);
    Character->ClearHitItem();
}

void UWFWeaponComponent::DropItem() const
{
    if (!CurrentWeapon) return;

    const auto ItemMesh = CurrentWeapon->FindComponentByClass<USkeletalMeshComponent>();
    if (!ItemMesh) return;

    const FDetachmentTransformRules DetachmentTransformRules{EDetachmentRule::KeepWorld, true};
    ItemMesh->DetachFromComponent(DetachmentTransformRules);

    CurrentWeapon->OnItemStateChanged.Broadcast(EItemState::EIS_Falling);
    CurrentWeapon->ThrowWeapon();
}

void UWFWeaponComponent::GetPickupItem(AWFBaseItem* Item)
{
    const auto Weapon = Cast<AWFBaseWeapon>(Item);
    if (Weapon)
    {
        SwapWeapon(Weapon);
    }
}

AWFBaseCharacter* UWFWeaponComponent::GetCharacter() const
{
    return Cast<AWFBaseCharacter>(GetOwner());
}
