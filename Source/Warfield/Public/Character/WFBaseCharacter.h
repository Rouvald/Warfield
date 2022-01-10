// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WFBaseCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UAnimMontage;
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

    FORCEINLINE bool GetIsAiming() const { return bIsAiming; }

    UFUNCTION(BlueprintCallable)
    float GetCurrentCrossHairSpread() const { return CurrentCrossHairSpread; }

protected:
    virtual void BeginPlay() override;

private:
    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess = "true"))
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess = "true"))
    UCameraComponent* FollowCamera;

    // Weapon
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon", meta=(AllowPrivateAccess = "true"))
    UAnimMontage* FireRecoilAnimMontage;

    /* Weapon Shooting*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon", meta=(AllowPrivateAccess = "true"))
    float TraceDistance = 20000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon", meta=(AllowPrivateAccess = "true"))
    float ShootTimeRate = 0.1;

    FTimerHandle ShootTimerHandle;

    bool bIsButtonFirePressed{false};
    bool bCanFire{true};

    /* Weapon VFX*/
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon VFX", meta=(AllowPrivateAccess = "true"))
    FName WeaponMuzzleFXSocketName = "BarrelSocket";

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon VFX", meta=(AllowPrivateAccess = "true"))
    UParticleSystem* MuzzleFX;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon VFX", meta=(AllowPrivateAccess = "true"))
    USoundCue* FireSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon VFX", meta=(AllowPrivateAccess = "true"))
    UParticleSystem* ImpactFX;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon VFX", meta=(AllowPrivateAccess = "true"))
    UParticleSystem* NoHitImpactFX;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon VFX", meta=(AllowPrivateAccess = "true"))
    UParticleSystem* TraceFX;
    //  

    // Zoom
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Zoom", meta=(AllowPrivateAccess = "true"))
    float ZoomAngleFOV{60.0f};

    float DefaultAngleFOV{0.0f};
    float CurrentAngleFOV{0.0f};

    bool bIsAiming{false};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Zoom", meta=(AllowPrivateAccess = "true"))
    float ZoomInterpSpeed{20.0f};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Zoom",
        meta=(AllowPrivateAccess = "true", ClampMin="0.1", ClampMax="2.0", UIMin="0.1", UIMax="2.0"))
    float DefaultMouseSensitivity{1.0f};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Zoom",
        meta=(AllowPrivateAccess = "true", ClampMin="0.1", ClampMax="2.0", UIMin="0.1", UIMax="2.0"))
    float ZoomMouseSensitivity{0.5f};
    //

    /* CrossHair */
    float CurrentCrossHairSpread{0.0f};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="CrossHair",
        meta=(AllowPrivateAccess = "true", ClampMin="0.0", ClampMax="1.5", UIMin="0.0", UIMax="1.5", ToolTip="CrossHair default scale"))
    float DefaultCrossHairSpread{1.0f};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="CrossHair", meta=(AllowPrivateAccess = "true"))
    FVector2D CrossHairSpreadRange{0.75f, 2.0f};

    /* CrossHair Velocity*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="CrossHair",
        meta=(AllowPrivateAccess = "true", ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0"))
    float CrossHairVelocitySpread{0.5f};

    /* CrossHair In Air*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="CrossHair",
        meta=(AllowPrivateAccess = "true", ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0"))
    float CrossHairInAirSpread{0.5f};

    float EditCrossHairAimSpread{0.0f};

    /* CrossHair Aiming*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="CrossHair",
        meta=(AllowPrivateAccess = "true", ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0"))
    float CrossHairAimSpread{0.5f};

    float EditCrossHairInAirSpread{0.0f};

    /* CrossHair Shooting*/
    FTimerHandle CrossHairShootTimerHandle;
    bool bIsShootingBullet{false};
    float ShootingTimeDuration{0.5f};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="CrossHair",
        meta=(AllowPrivateAccess = "true", ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0"))
    float CrossHairShootingSpread{0.25f};

    float EditCrossHairShootingSpread{0.0f};
    //

    bool bIsHit = false;

    void MoveForward(const float Value);
    void MoveRight(const float Value);

    virtual void AddControllerYawInput(float Val) override;
    virtual void AddControllerPitchInput(float Val) override;
    float GetCurrentMouseSensitivity(const float DefaultMouseSenseVal) const;

    void StartFire();
    void StartFireTimer();
    void ResetFireTimer();
    void StopFire();

    void MakeShot();

    bool GetTraceData(FVector& TraceStart, FVector& TraceEnd) const;

    void MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd, FVector& TraceFXEnd);
    void MakeWeaponHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd, FVector& TraceFXEnd);

    void PlayFireRecoilAnimMon() const;

    void InitFX() const;
    void SpawnImpactFX(const FVector& TraceFXEnd) const;
    void SpawnTraceFX(const FVector& TraceFXStart, const FVector& TraceFXEnd) const;

    FVector GetSocketLocation() const;

    void Zoom(const bool bEnable);
    void UpdateZoomInterp(const float DeltaTime);

    void UpdateCrossHairSpread(const float DeltaTime);
    float UpdateCrossHairVelocitySpread() const;
    void UpdateInterpCrossHairSpread(const bool bIsEnable, float& EditCrossHairSpread, const float CrossHairSpread, const float DeltaTime,
        const float InterpSpeedEnable = 30.0f, const float InterpSpeedNOTEnable = 30.0f);

    void StartCrossHairShoot();

    UFUNCTION()
    void StopCrossHairShoot();
};
