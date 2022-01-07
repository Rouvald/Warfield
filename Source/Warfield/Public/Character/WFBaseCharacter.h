// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WFBaseCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UNiagaraSystem;
class USoundCue;

UCLASS()
class WARFIELD_API AWFBaseCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AWFBaseCharacter();

    virtual void Tick(float DeltaTime) override;

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

    FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
    virtual void BeginPlay() override;

    void FireWeapon();

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess = "true"))
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess = "true"))
    UCameraComponent* FollowCamera;

    // VFX
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Sound", meta=(AllowPrivateAccess = "true"))
    FName WeaponMuzzFXleSocketName = "BarrelSocket";

    /*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sound", meta=(AllowPrivateAccess = "true"))
    UNiagaraSystem* MuzzleFX;*/

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sound", meta=(AllowPrivateAccess = "true"))
    UParticleSystem* MuzzleFX;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sound", meta=(AllowPrivateAccess = "true"))
    USoundCue* FireSound;

    void MoveForward(const float Value);
    void MoveRight(const float Value);
};
