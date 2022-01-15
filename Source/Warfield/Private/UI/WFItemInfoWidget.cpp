// Warfield Game. All Rights Reserved

#include "UI/WFItemInfoWidget.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Blueprint/WidgetTree.h"

DEFINE_LOG_CATEGORY_STATIC(LogWFItemInfoWidget, All, All)

void UWFItemInfoWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (ItemName)
    {
        ItemName->SetVisibility(ESlateVisibility::Visible);
    }
    if (ItemCount)
    {
        ItemCount->SetVisibility(ESlateVisibility::Visible);
    }
    if (ItemRarityBox && WidgetTree)
    {
        ItemRarityBox->ClearChildren();
        for (int8 Index{0}; Index < ItemRarityImageCount; ++Index)
        {
            const auto ItemRarityWidget = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), FName{TEXT("Star"), Index + 1});
            if (ItemRarityWidget)
            {
                ItemRarityWidget->SetBrushFromTexture(ItemRarityImage);
                //ItemRarityWidget->SetOpacity(0.0f);
                ItemRarityWidget->SetVisibility(ESlateVisibility::Hidden);
                ItemRarityBox->AddChild(ItemRarityWidget);
            }
        }
    }
}

void UWFItemInfoWidget::SetItemName(const FName Name) const
{
    if (ItemName)
    {
        ItemName->SetText(FText::FromName(Name));
    }
}

void UWFItemInfoWidget::SetItemCount(const int32 Count) const
{
    if (ItemCount)
    {
        ItemCount->SetText(FText::FromString(FString::FromInt(Count)));
    }
}

void UWFItemInfoWidget::SetItemRarityVisibility(const int8 Rarity) const
{
    const auto RarityWidgets = ItemRarityBox->GetAllChildren();
    if(RarityWidgets.Num() == 0) return;

    if(Rarity < 0) return;

    for(int8 Index{0}; Index < Rarity; ++Index)
    {
        RarityWidgets[Index]->SetVisibility(ESlateVisibility::Visible);
    }
}
