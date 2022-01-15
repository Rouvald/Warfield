// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WFCoreTypes.h"
#include "WFBaseCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UWFCrossHairComponent;
class UWFWeaponComponent;
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

    FORCEINLINE AWFBaseItem* GetHitItem() const {return HitItem;}
    /* @todo: Replace this method.*/
    FORCEINLINE void SetHitItem(AWFBaseItem* NewHitItem) {HitItem = NewHitItem;}

protected:
    virtual void BeginPlay() override;

private:
    /* Components */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FollowCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UWFCrossHairComponent* CrossHairComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UWFWeaponComponent* WeaponComponent;

    /* Interaction with Items*/
    FTimerHandle ItemInfoVisibilityTimerHandle;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    float ItemVisibilityTraceDistance = 5000.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    AWFBaseItem* HitItem{nullptr};
    
    UPROPERTY()
    TArray<const AWFBaseItem*> HittedItems;
    //
    
    void MoveForward(const float Value);
    void MoveRight(const float Value);

    virtual void AddControllerYawInput(float Val) override;
    virtual void AddControllerPitchInput(float Val) override;

    void ItemInfoVisibilityTimer(const AWFBaseItem* Item, bool bIsOverlap);
    void UpdateItemInfoVisibility();
    bool MakeHitItemVisibility(FHitResult& HitResult);
    void HideAllHittedItems() const;
};
