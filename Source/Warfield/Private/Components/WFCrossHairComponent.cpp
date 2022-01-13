// Warfield Game. All Rigths Reserved

#include "Components/WFCrossHairComponent.h"
#include "Character/WFBaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UWFCrossHairComponent::UWFCrossHairComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UWFCrossHairComponent::BeginPlay()
{
    Super::BeginPlay();

    const auto Character = GetCharacter();
    if (Character)
    {
        const auto FollowCamera = Character->FindComponentByClass<UCameraComponent>();
        if (FollowCamera)
        {
            DefaultAngleFOV = FollowCamera->FieldOfView;
            CurrentAngleFOV = FollowCamera->FieldOfView;
        }
    }
}

void UWFCrossHairComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateZoomInterp(DeltaTime);
    UpdateCrossHairSpread(DeltaTime);
}

void UWFCrossHairComponent::Zoom(const bool bEnable)
{
    bIsAiming = bEnable;
}

void UWFCrossHairComponent::UpdateZoomInterp(const float DeltaTime)
{
    const auto Character = GetCharacter();
    if (!Character) return;

    const auto FollowCamera = Character->FindComponentByClass<UCameraComponent>();
    if (!FollowCamera) return;

    if (bIsAiming)
    {
        CurrentAngleFOV = FMath::FInterpTo(CurrentAngleFOV, ZoomAngleFOV, DeltaTime, ZoomInterpSpeed);
    }
    else
    {
        CurrentAngleFOV = FMath::FInterpTo(CurrentAngleFOV, DefaultAngleFOV, DeltaTime, ZoomInterpSpeed);
    }
    FollowCamera->SetFieldOfView(CurrentAngleFOV);
}

float UWFCrossHairComponent::GetCurrentMouseSensitivity(const float DefaultMouseSenseVal) const
{
    if (bIsAiming)
    {
        return ZoomMouseSensitivity * DefaultMouseSenseVal;
    }
    return DefaultMouseSenseVal * DefaultMouseSensitivity;
}

void UWFCrossHairComponent::UpdateCrossHairSpread(const float DeltaTime)
{
    const auto Character = GetCharacter();
    if (!Character || !Character->GetCharacterMovement()) return;
    // Velocity Spread
    const auto EditCrossHairVelocitySpread{UpdateCrossHairVelocitySpread()};

    // In air Spread
    UpdateInterpCrossHairSpread(
        Character->GetCharacterMovement()->IsFalling(), EditCrossHairInAirSpread, CrossHairInAirSpread, DeltaTime, 1.0f);

    // Aim Spread
    UpdateInterpCrossHairSpread(bIsAiming, EditCrossHairAimSpread, CrossHairAimSpread, DeltaTime);

    // Shoot Spread
    UpdateInterpCrossHairSpread(bIsShootingBullet, EditCrossHairShootingSpread, CrossHairShootingSpread, DeltaTime, 60.0f, 60.0f);

    // Total Current Spread
    CurrentCrossHairSpread = FMath::Clamp(DefaultCrossHairSpread + EditCrossHairVelocitySpread + EditCrossHairInAirSpread -
                                              EditCrossHairAimSpread + EditCrossHairShootingSpread, //
        //
        CrossHairSpreadRange.X, //
        CrossHairSpreadRange.Y  //
    );
    //
}

float UWFCrossHairComponent::UpdateCrossHairVelocitySpread() const
{
    const auto Character = GetCharacter();
    if (!Character || !Character->GetCharacterMovement()) return 0.0f;

    const FVector2D Velocity2D{0.0f, Character->GetCharacterMovement()->GetMaxSpeed()};
    const FVector2D Spread2D{0.0f, CrossHairVelocitySpread};
    FVector Velocity = Character->GetVelocity();
    Velocity.Z = 0.0f;

    return FMath::GetMappedRangeValueClamped(Velocity2D, Spread2D, Velocity.Size());
}

void UWFCrossHairComponent::UpdateInterpCrossHairSpread(const bool bIsEnable, float& EditCrossHairSpread, const float CrossHairSpread,
    const float DeltaTime, const float InterpSpeedEnable, const float InterpSpeedNOTEnable)
{
    if (bIsEnable)
    {
        EditCrossHairSpread = FMath::FInterpTo(EditCrossHairSpread, CrossHairSpread, DeltaTime, InterpSpeedEnable);
    }
    else
    {
        EditCrossHairSpread = FMath::FInterpTo(EditCrossHairSpread, 0.0f, DeltaTime, InterpSpeedNOTEnable);
    }
}

void UWFCrossHairComponent::StartCrossHairShoot()
{
    if (!GetWorld()) return;

    bIsShootingBullet = true;
    GetWorld()->GetTimerManager().SetTimer(
        CrossHairShootTimerHandle, this, &UWFCrossHairComponent::StopCrossHairShoot, ShootingTimeDuration, false);
}

void UWFCrossHairComponent::StopCrossHairShoot()
{
    bIsShootingBullet = false;
}

AWFBaseCharacter* UWFCrossHairComponent::GetCharacter() const
{
    return Cast<AWFBaseCharacter>(GetOwner());
}
