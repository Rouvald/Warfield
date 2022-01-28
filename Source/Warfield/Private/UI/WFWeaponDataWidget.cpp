// Warfield Game. All Rights Reserved


#include "UI/WFWeaponDataWidget.h"

#include "WFBaseCharacter.h"
#include "WFBaseWeapon.h"
#include "WFUtils.h"
#include "WFWeaponComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UWFWeaponDataWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (WeaponName)
    {
        WeaponName->SetVisibility(ESlateVisibility::Visible);
    }
    if (WeaponAmmo)
    {
        WeaponAmmo->SetVisibility(ESlateVisibility::Visible);
    }
    if (BulletsImage)
    {
        BulletsImage->SetVisibility(ESlateVisibility::Visible);
    }
}

int32 UWFWeaponDataWidget::GetAmmoData() const
{
    const auto WeaponComponent = GetWeaponComponent();
    if (!WeaponComponent || !WeaponComponent->GetCurrentWeapon())return 0;

    return WeaponComponent->GetCurrentWeapon()->GetCurrentBulletAmount();
}

FText UWFWeaponDataWidget::GetWeaponName() const
{
    const auto WeaponComponent = GetWeaponComponent();
    if (!WeaponComponent || !WeaponComponent->GetCurrentWeapon()) return FText{};

    return FText::FromName(WeaponComponent->GetCurrentWeapon()->GetItemName());
}

UTexture2D* UWFWeaponDataWidget::GetBulletsImage() const
{
    const auto WeaponComponent = GetWeaponComponent();
    if (!WeaponComponent || !WeaponComponent->GetCurrentWeapon()) return nullptr;

    return WeaponComponent->GetCurrentWeapon()->GetAmmoTypeBulletTexture();
}

UWFWeaponComponent* UWFWeaponDataWidget::GetWeaponComponent() const
{
    const auto Character = Cast<AWFBaseCharacter>(GetOwningPlayerPawn());
    if (!Character) return nullptr;

    const auto WeaponComponent = Character->FindComponentByClass<UWFWeaponComponent>();
    return WeaponComponent;
}
