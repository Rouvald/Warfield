// Warfield Game. All Rigths Reserved

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WFCoreTypes.h"
#include "WFAnimInstance.generated.h"

class AWFBaseCharacter;
/**
 *
 */
UCLASS()
class WARFIELD_API UWFAnimInstance : public UAnimInstance
{
    GENERATED_BODY()
public:
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable)
    void UpdateAnimationProperties(float DeltaTime);

    virtual void NativeInitializeAnimation() override;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    AWFBaseCharacter* BaseCharacter{nullptr};

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    float Speed{0.0f};

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    bool bIsInAir{false};

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    bool bIsAccelerating{false};

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    float MovementDirection{0.0f};

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    float LastMovementDirection{0.0f};

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    bool bIsButtonFirePressed{false};

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    bool bIsAiming{false};

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    bool bIsReloading{false};

    float TIPCurrentCharacterYaw{0.0f};
    float TIPLastFrameCharacterYaw{0.0f};

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Turn in place", meta = (AllowPrivateAccess = "true"))
    float RootYawOffset{0.0f};

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Turn in place", meta = (AllowPrivateAccess = "true"))
    float PitchOffset{0.0f};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn in place", meta = (AllowPrivateAccess = "true"))
    float MaxYawRotation{90.0f};

    float RotationCurve{0.0f};
    float RotationCurveLastFrame{0.0f};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn in place", meta = (AllowPrivateAccess = "true"))
    EOffsetState CurrentOffsetState{EOffsetState::EOS_Hip};

    FRotator LeanCurrentCharacterRotator{FRotator::ZeroRotator};
    FRotator LeanLastFrameCharacterRotator{FRotator::ZeroRotator};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lean", meta = (AllowPrivateAccess = "true"))
    float LeanDeltaYaw{0.0f};

    void UpdateMovementDirection();
    void TurnInPlace();

    void Lean(float DeltaTime);

    void SetCurrentOffsetState();
};
