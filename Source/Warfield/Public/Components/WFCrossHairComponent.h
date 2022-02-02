// Warfield Game. All Rigths Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WFCrossHairComponent.generated.h"

class AWFBaseCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class WARFIELD_API UWFCrossHairComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UWFCrossHairComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void Zoom(const bool bEnable);
    float GetCurrentMouseSensitivity(const float DefaultMouseSenseVal) const;

    void StartCrossHairShoot();

    FORCEINLINE bool GetIsAiming() const { return bIsAiming; }

    UFUNCTION(BlueprintCallable)
    float GetCurrentCrossHairSpread() const { return CurrentCrossHairSpread; }

protected:
    virtual void BeginPlay() override;

private:
    // Zoom
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoom", meta = (AllowPrivateAccess = "true"))
    float ZoomAngleFOV{60.0f};

    float DefaultAngleFOV{0.0f};
    float CurrentAngleFOV{0.0f};

    bool bIsAiming{false};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoom", meta = (AllowPrivateAccess = "true"))
    float ZoomInterpSpeed{20.0f};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoom",
        meta = (AllowPrivateAccess = "true", ClampMin = "0.1", ClampMax = "2.0", UIMin = "0.1", UIMax = "2.0"))
    float DefaultMouseSensitivity{1.0f};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoom",
        meta = (AllowPrivateAccess = "true", ClampMin = "0.1", ClampMax = "2.0", UIMin = "0.1", UIMax = "2.0"))
    float ZoomMouseSensitivity{0.5f};
    //

    /* CrossHair */
    float CurrentCrossHairSpread{0.0f};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CrossHair",
        meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.5", UIMin = "0.0", UIMax = "1.5",
            ToolTip = "CrossHair default scale"))
    float DefaultCrossHairSpread{1.0f};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CrossHair", meta = (AllowPrivateAccess = "true"))
    FVector2D CrossHairSpreadRange{0.75f, 2.0f};

    /* CrossHair Velocity*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CrossHair",
        meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float CrossHairVelocitySpread{0.5f};

    /* CrossHair In Air*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CrossHair",
        meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float CrossHairInAirSpread{0.5f};

    float EditCrossHairAimSpread{0.0f};

    /* CrossHair Aiming*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CrossHair",
        meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float CrossHairAimSpread{0.5f};

    float EditCrossHairInAirSpread{0.0f};

    /* CrossHair Shooting*/
    FTimerHandle CrossHairShootTimerHandle;
    bool bIsShootingBullet{false};
    float ShootingTimeDuration{0.5f};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CrossHair",
        meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float CrossHairShootingSpread{0.25f};

    float EditCrossHairShootingSpread{0.0f};
    //

    void UpdateZoomInterp(const float DeltaTime);

    void UpdateCrossHairSpread(const float DeltaTime);
    float UpdateCrossHairVelocitySpread() const;
    void UpdateInterpCrossHairSpread(const bool bIsEnable, float& EditCrossHairSpread, const float CrossHairSpread, const float DeltaTime,
        const float InterpSpeedEnable = 30.0f, const float InterpSpeedNOTEnable = 30.0f);

    UFUNCTION()
    void StopCrossHairShoot();

    void UpdateIsAiming();

    AWFBaseCharacter* GetCharacter() const;
};
