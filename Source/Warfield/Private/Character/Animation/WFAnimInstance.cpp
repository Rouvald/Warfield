// Warfield Game. All Rigths Reserved


#include "Character/Animation/WFAnimInstance.h"
#include "WFBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UWFAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
    if (!BaseCharacter)
    {
        BaseCharacter = Cast<AWFBaseCharacter>(TryGetPawnOwner());
    }
    if (BaseCharacter)
    {
        auto Velosity{BaseCharacter->GetVelocity()};
        Velosity.Z = 0.0f;
        Speed = Velosity.Size();

        bIsInAir = BaseCharacter->GetCharacterMovement()->IsFalling();

        BaseCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f
            ? bIsAccelerating = true
            : bIsAccelerating = false;
    }
}

void UWFAnimInstance::NativeInitializeAnimation()
{
    BaseCharacter = Cast<AWFBaseCharacter>(TryGetPawnOwner());
}
