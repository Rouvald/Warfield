// Warfield Game. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "WFGameHUD.generated.h"

class UWFCrossHairComponent;
/**
 * 
 */
UCLASS()
class WARFIELD_API AWFGameHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI", meta= (AllowPrivateAccess = "true"))
    TSubclassOf<UUserWidget> PlayerWidgetClass;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI", meta= (AllowPrivateAccess = "true"))
    UTexture2D* DotCrossHairTexture;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI", meta= (AllowPrivateAccess = "true"))
    TArray<UTexture2D*> CrossHairTextures;

    UPROPERTY()
    UUserWidget* PlayerWidget{nullptr};

    UPROPERTY()
    UWFCrossHairComponent* CrossHairComponent;

    void DrawCrossHair(UTexture2D* CurrentTexture);
    
    UWFCrossHairComponent* GetCrossHairComponent() const;
    float GetCurrentCrossHairSpread() const;
};
