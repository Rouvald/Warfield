// Warfield Game. All Rigths Reserved

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
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
    bool bIsAiming{false};

    void UpdateMovementDirection();
};
