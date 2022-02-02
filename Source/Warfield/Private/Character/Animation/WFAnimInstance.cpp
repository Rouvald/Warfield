// Warfield Game. All Rigths Reserved

#include "Character/Animation/WFAnimInstance.h"
#include "WFBaseCharacter.h"
#include "WFBaseWeapon.h"
#include "WFWeaponComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/WFCrossHairComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogWFAnimInstance, All, All);

void UWFAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    UpdateAnimationProperties(DeltaSeconds);
}

void UWFAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
    if (!BaseCharacter)
    {
        BaseCharacter = Cast<AWFBaseCharacter>(TryGetPawnOwner());
    }
    if (BaseCharacter)
    {
        auto Velocity{BaseCharacter->GetVelocity()};
        Velocity.Z = 0.0f;
        Speed = Velocity.Size();

        bIsInAir = BaseCharacter->GetCharacterMovement()->IsFalling();

        BaseCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f ? bIsAccelerating = true : bIsAccelerating = false;

        UpdateMovementDirection();

        const auto CrossHairComponent = BaseCharacter->FindComponentByClass<UWFCrossHairComponent>();
        if (CrossHairComponent)
        {
            bIsAiming = CrossHairComponent->GetIsAiming();
        }
        const auto WeaponComponent = BaseCharacter->FindComponentByClass<UWFWeaponComponent>();
        if (WeaponComponent && WeaponComponent->GetCurrentWeapon())
        {
            bIsButtonFirePressed = WeaponComponent->GetCurrentWeapon()->GetIsButtonFirePressed();
            bIsReloading = WeaponComponent->GetCurrentWeapon()->GetCurrentWeaponState() == EWeaponState::EWS_Reloading;
        }
    }
    TurnInPlace();
    SetCurrentOffsetState();
    Lean(DeltaTime);
}

void UWFAnimInstance::NativeInitializeAnimation()
{
    BaseCharacter = Cast<AWFBaseCharacter>(TryGetPawnOwner());
}

void UWFAnimInstance::UpdateMovementDirection()
{
    const FRotator AimRotation{BaseCharacter->GetBaseAimRotation()};
    const FRotator MovementRotation{UKismetMathLibrary::MakeRotFromX(BaseCharacter->GetVelocity())};

    MovementDirection = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

    if (BaseCharacter->GetVelocity().Size() > 0.0f)
    {
        LastMovementDirection = MovementDirection;
    }
}

void UWFAnimInstance::TurnInPlace()
{
    if (!BaseCharacter) return;

    PitchOffset = BaseCharacter->GetBaseAimRotation().Pitch;

    if (FMath::IsNearlyZero(Speed) && !bIsInAir)
    {
        TIPLastFrameCharacterYaw = TIPCurrentCharacterYaw;
        TIPCurrentCharacterYaw = BaseCharacter->GetActorRotation().Yaw;

        const auto TIPDeltaYaw{TIPCurrentCharacterYaw - TIPLastFrameCharacterYaw};

        RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - TIPDeltaYaw);

        const float Turning{GetCurveValue(TEXT("Turning"))};
        if (Turning > 0)
        {
            RotationCurveLastFrame = RotationCurve;
            RotationCurve = GetCurveValue(TEXT("Rotation"));
            const auto DeltaRotation{RotationCurve - RotationCurveLastFrame};

            RootYawOffset > 0 ? RootYawOffset -= DeltaRotation : RootYawOffset += DeltaRotation;

            const auto ABSRootYawOffset{FMath::Abs(RootYawOffset)};
            if (ABSRootYawOffset > MaxYawRotation)
            {
                const auto YawExcess{ABSRootYawOffset - MaxYawRotation};
                RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
            }
        }
    }
    else
    {
        RootYawOffset = 0.0f;
        TIPCurrentCharacterYaw = BaseCharacter->GetActorRotation().Yaw;
        TIPLastFrameCharacterYaw = TIPCurrentCharacterYaw;

        RotationCurve = 0.0f;
        RotationCurveLastFrame = 0.0f;
    }
}

void UWFAnimInstance::Lean(float DeltaTime)
{
    if (!BaseCharacter) return;

    LeanLastFrameCharacterRotator = LeanCurrentCharacterRotator;
    LeanCurrentCharacterRotator = BaseCharacter->GetActorRotation();
    const FRotator DeltaRotator {UKismetMathLibrary::NormalizedDeltaRotator(LeanCurrentCharacterRotator, LeanLastFrameCharacterRotator)};
    
    const auto Target{ DeltaRotator.Yaw / DeltaTime};
    const float Interp {FMath::FInterpTo(LeanDeltaYaw, Target, DeltaTime, 6.0f)};

    LeanDeltaYaw = FMath::Clamp(Interp, -90.0f, 90.0f);

    //if (GEngine) GEngine->AddOnScreenDebugMessage(1, -1, FColor::Red, FString::Printf(TEXT("Lean delta Yaw: %f"), LeanDeltaYaw));
}

void UWFAnimInstance::SetCurrentOffsetState()
{
    if (bIsReloading)
    {
        CurrentOffsetState = EOffsetState::EOS_Reloading;
    }
    else if (bIsInAir)
    {
        CurrentOffsetState = EOffsetState::EOS_InAir;
    }
    else if (bIsAiming || bIsButtonFirePressed)
    {
        CurrentOffsetState = EOffsetState::EOS_Aiming;
    }
    else
    {
        CurrentOffsetState = EOffsetState::EOS_Hip;
    }
}
