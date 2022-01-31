// Warfield Game. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WFCoreTypes.h"
#include "WFBaseItem.generated.h"

class AWFBaseCharacter;
class UBoxComponent;
class USphereComponent;
class UWidgetComponent;
class UWFItemInfoWidget;
class UCurveFloat;
class USoundCue;

UCLASS()
class WARFIELD_API AWFBaseItem : public AActor
{
    GENERATED_BODY()

public:
    AWFBaseItem();

    FOnItemStateChangedSignature OnItemStateChanged;

    virtual void Tick(float DeltaTime) override;

    void StartItemInterping(AWFBaseCharacter* Character);

    FORCEINLINE FName GetItemName() const { return ItemName; }
    FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }

    FORCEINLINE USoundCue* GetPickupSound() const { return PickupSound;}
    FORCEINLINE USoundCue* GetEquipSound() const { return EquipSound;}

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* ItemMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UBoxComponent* BoxCollision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USphereComponent* AreaCollision;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UWidgetComponent* ItemInfoWidgetComponent;


    /* Item Properties*/
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
    FName ItemName{"BaseItem"};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
    int32 ItemCount{0};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
    EItemRarity ItemRarity{EItemRarity::EIR_Damaged};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
    UCurveFloat* ItemZCurve;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
    float ItemZCurveTime{0.7f};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
    UCurveFloat* ItemScaleCurve;

    FTimerHandle ItemInterpingTimerHandle;
    bool bIsItemInterping{false};
//
    /* Item Sounds */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Sound", meta = (AllowPrivateAccess = "true"))
    USoundCue* PickupSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Sound", meta = (AllowPrivateAccess = "true"))
    USoundCue* EquipSound;
    
    //
    UPROPERTY()
    AWFBaseCharacter* BaseCharacter{nullptr};

    UPROPERTY()
    FVector ItemBaseLocation{FVector::ZeroVector};
    float ItemInterpSpeed{30.0f};
    //float DefaultRotationYawOffset{0.0f};

    EItemState CurrentItemState{EItemState::EIS_Pickup};

    UPROPERTY()
    TMap<EItemState, FItemProperties> ItemPropertiesMap;
    //

    //
    virtual void BeginPlay() override;
    //

    void SetItemInfo() const;
    void SetItemRarity(const UWFItemInfoWidget* ItemInfoWidget) const;

    void SetItemState(EItemState NewItemState);

    void FillItemPropertiesMap();
    void SetItemProperties(EItemState NewItemState) const;

    UFUNCTION()
    void OnAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnAreaEndOverlap(
        UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    void ItemIterping(float DeltaTime);
    void ItemInterpXY(FVector& ItemLocation, float DeltaTime);
    void ItemInterpZ(FVector& ItemLocation);
    void ItemInterpRotationYaw();
    void ItemInterpScale();

    void FinishItemInterping();
};
