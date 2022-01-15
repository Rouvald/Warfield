// Warfield Game. All Rigths Reserved

#include "Components/WFWeaponComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

#include "WFBaseCharacter.h"
#include "WFBaseWeapon.h"

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
    const auto Character = GetBaseCharacter();
    if(!Character || !Character->GetHitItem()) return;

    const auto Weapon = Cast<AWFBaseWeapon>(Character->GetHitItem());
    
    SwapWeapon(Weapon);
}

void UWFWeaponComponent::TakeWeaponButtonReleased()
{
    
}

void UWFWeaponComponent::DropWeaponButtonPressed()
{
    //DropWeapon();
}

void UWFWeaponComponent::DropWeaponButtonReleased()
{
    
}

AWFBaseWeapon* UWFWeaponComponent::SpawnWeapon() const
{
    const auto Character = GetBaseCharacter();
    if (!GetWorld() || !Character || !DefaultWeaponClass) return nullptr;

    const auto DefaultWeapon = GetWorld()->SpawnActor<AWFBaseWeapon>(DefaultWeaponClass);
    return DefaultWeapon;
}

void UWFWeaponComponent::EquipWeapon(AWFBaseWeapon* EquippedWeapon)
{
    if (!EquippedWeapon) return;

    const auto Character = GetBaseCharacter();
    if(!Character) return;

    EquippedWeapon->SetOwner(Character);

    AttachWeaponToComponent(EquippedWeapon, Character->GetMesh(), WeaponSocketName);

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
    const auto Character = GetBaseCharacter();
    if(!Character) return;
    
    DropWeapon();
    EquipWeapon(NewWeapon);
    Character->SetHitItem(nullptr);
}

void UWFWeaponComponent::DropWeapon() const
{
    if(!CurrentWeapon) return;
    
    const auto ItemMesh = CurrentWeapon->FindComponentByClass<USkeletalMeshComponent>();
    if(!ItemMesh) return;
    
    const FDetachmentTransformRules DetachmentTransformRules{EDetachmentRule::KeepWorld, true};
    ItemMesh->DetachFromComponent(DetachmentTransformRules);

    CurrentWeapon->OnItemStateChanged.Broadcast(EItemState::EIS_Falling);
    CurrentWeapon->ThrowWeapon();
}

AWFBaseCharacter* UWFWeaponComponent::GetBaseCharacter() const
{
    return Cast<AWFBaseCharacter>(GetOwner());
}
