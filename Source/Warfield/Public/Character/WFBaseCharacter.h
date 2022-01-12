// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WFCoreTypes.h"
#include "WFBaseCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UWFCrossHairComponent;
class UAnimMontage;
class UNiagaraSystem;
class USoundCue;
class AWFBaseItem;

UCLASS()
class WARFIELD_API AWFBaseCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    FOnItemAreaOverlapSignature OnItemAreaOverlap;
    
    AWFBaseCharacter();

    virtual void Tick(float DeltaTime) override;

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    /*
    FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
    FORCEINLINE UWFCrossHairComponent* GetCrossHairComponent() const { return CrossHairComponent; }
    */

protected:
    virtual void BeginPlay() override;

private:
    /* Components */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess = "true"))
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess = "true"))
    UCameraComponent* FollowCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess = "true"))
    UWFCrossHairComponent* CrossHairComponent;

    // Weapon
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon", meta=(AllowPrivateAccess = "true"))
    UAnimMontage* FireRecoilAnimMontage;

    /* Weapon Shooting*/
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon", meta=(AllowPrivateAccess = "true"))
    float ShootTraceDistance = 20000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon", meta=(AllowPrivateAccess = "true"))
    float ShootTimeRate = 0.6;

    FTimerHandle ShootTimerHandle;

    bool bIsButtonFirePressed{false};
    bool bCanFire{true};

    /* Weapon VFX*/
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon VFX", meta=(AllowPrivateAccess = "true"))
    FName WeaponMuzzleFXSocketName = "MuzzleSocket";

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

    /* Interaction with Items*/
    FTimerHandle ItemInfoVisibilityTimerHandle;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon", meta=(AllowPrivateAccess = "true"))
    float ItemVisibilityTraceDistance = 5000.0f;
    //

    /* Item is overlap*/
    //int8 ItemOverlapCounter{0};
    UPROPERTY()
    TArray<const AWFBaseItem*> HittedItems;
    //

    bool bIsHit = false;

    void MoveForward(const float Value);
    void MoveRight(const float Value);

    virtual void AddControllerYawInput(float Val) override;
    virtual void AddControllerPitchInput(float Val) override;

    void StartFire();
    void StartFireTimer();
    void ResetFireTimer();
    void StopFire();

    void MakeShot();

    bool GetTraceData(FVector& TraceStart, FVector& TraceEnd, const float TraceDistance) const;

    void MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd, FVector& TraceFXEnd);

    void PlayFireRecoilAnimMon() const;

    void InitFX() const;
    void SpawnImpactFX(const FHitResult& HitResult, const FVector& TraceFXEnd) const;
    void SpawnTraceFX(const FVector& TraceFXStart, const FVector& TraceFXEnd) const;

    FVector GetSocketLocation() const;

    void ItemInfoVisibilityTimer(const AWFBaseItem* Item, bool bIsOverlap);
    void UpdateItemInfoVisibility();
    bool MakeHitItemVisibility(FHitResult& HitResult);

    void HideAllHittedItems()const;
};
