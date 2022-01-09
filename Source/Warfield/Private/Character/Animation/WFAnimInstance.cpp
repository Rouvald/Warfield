// Warfield Game. All Rigths Reserved


#include "Character/Animation/WFAnimInstance.h"
#include "WFBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

//DEFINE_LOG_CATEGORY_STATIC(LogWFAnimInstance, All, All);

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

        BaseCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f
            ? bIsAccelerating = true
            : bIsAccelerating = false;

        UpdateMovementDirection();
    }
}

void UWFAnimInstance::NativeInitializeAnimation()
{
    BaseCharacter = Cast<AWFBaseCharacter>(TryGetPawnOwner());
}

void UWFAnimInstance::UpdateMovementDirection()
{
    const FRotator AimRotation {BaseCharacter->GetBaseAimRotation()};
    const FRotator MovementRotation {UKismetMathLibrary::MakeRotFromX(BaseCharacter->GetVelocity())};
    
    MovementDirection = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

    if(BaseCharacter->GetVelocity().Size() > 0.0f)
    {
        LastMovementDirection = MovementDirection;
    }
}