// Warfield Game. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "WFCoreTypes.h"
#include "Blueprint/UserWidget.h"
#include "WFWeaponDataWidget.generated.h"

class UWFWeaponComponent;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class WARFIELD_API UWFWeaponDataWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="UI")
    FText GetAmmoData() const;

    UFUNCTION(BlueprintCallable, Category="UI")
    FText GetWeaponName() const;

    UFUNCTION(BlueprintCallable, Category="UI")
    UTexture2D* GetBulletsImage() const;

protected:
    virtual void NativeOnInitialized() override;

private:
    UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
    UTextBlock* WeaponName;

    UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
    UTextBlock* WeaponAmmo;

    UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
    UImage* BulletsImage;

    UWFWeaponComponent* GetWeaponComponent() const;
};
