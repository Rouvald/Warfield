// Warfield Game. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WFItemInfoWidget.generated.h"

class AWFPlayerController;
class UTextBlock;
class UHorizontalBox;
class UImage;
/**
 *
 */
UCLASS()
class WARFIELD_API UWFItemInfoWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    void SetItemName(const FName Name) const;
    void SetItemCount(const int32 Count) const;
    void SetItemRarityVisibility(const int8 Rarity) const;

protected:
    virtual void NativeOnInitialized() override;

private:
    UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
    UTextBlock* ItemName;

    UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
    UTextBlock* ItemCount;

    UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
    UHorizontalBox* ItemRarityBox;

    int8 ItemRarityImageCount{5};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="UI", meta=(AllowPrivateAccess="true"))
    UTexture2D* ItemRarityImage;
};
