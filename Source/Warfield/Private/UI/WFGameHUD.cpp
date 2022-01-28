// Warfield Game. All Rights Reserved


#include "UI/WFGameHUD.h"

#include "WFBaseCharacter.h"
#include "WFCrossHairComponent.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Canvas.h"

void AWFGameHUD::DrawHUD()
{
    Super::DrawHUD();

    DrawCrossHair(DotCrossHairTexture);
    
    for (const auto CrossHairTexture : CrossHairTextures)
    {
        if (CrossHairTexture)
        {
            DrawCrossHair(CrossHairTexture);
        }
    }
}

void AWFGameHUD::BeginPlay()
{
    Super::BeginPlay();

    if (PlayerWidgetClass)
    {
        PlayerWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerWidgetClass);
        if (PlayerWidget)
        {
            PlayerWidget->AddToViewport();
            PlayerWidget->SetVisibility(ESlateVisibility::Visible);
        }
    }
    CrossHairComponent = GetCrossHairComponent();
}

void AWFGameHUD::DrawCrossHair(UTexture2D* CurrentTexture)
{
    if (!CurrentTexture) return;

    const FVector2D CenterLocation = FVector2D{Canvas->SizeX * 0.5f, Canvas->SizeY * 0.5f};
    const float CurrentCrossHairSpread{CurrentTexture == DotCrossHairTexture ? 1.0f : GetCurrentCrossHairSpread()};

    const FVector2D CrossHairTextureCenter{(CurrentTexture->GetSizeX() / 2.0f) * CurrentCrossHairSpread,
                                           (CurrentTexture->GetSizeY() / 2.0f) * CurrentCrossHairSpread};

    DrawTextureSimple
        (
            CurrentTexture,                              //
            CenterLocation.X - CrossHairTextureCenter.X, //
            CenterLocation.Y - CrossHairTextureCenter.Y, //
            CurrentCrossHairSpread                  //
            );
}

UWFCrossHairComponent* AWFGameHUD::GetCrossHairComponent() const
{
    const auto BaseCharacter = Cast<AWFBaseCharacter>(GetOwningPawn());
    if (!BaseCharacter) return nullptr;

    const auto NewCrossHairComponent = BaseCharacter->FindComponentByClass<UWFCrossHairComponent>();
    return NewCrossHairComponent;
}

float AWFGameHUD::GetCurrentCrossHairSpread() const
{
    if (!CrossHairComponent) return 0.0f;
    return CrossHairComponent->GetCurrentCrossHairSpread();
}
