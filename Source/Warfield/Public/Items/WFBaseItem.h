// Warfield Game. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WFCoreTypes.h"
#include "WFBaseItem.generated.h"

class UBoxComponent;
class USphereComponent;
class UWidgetComponent;
class UWFItemInfoWidget;

UCLASS()
class WARFIELD_API AWFBaseItem : public AActor
{
    GENERATED_BODY()

public:
    AWFBaseItem();

    FOnItemStateChangedSignature OnItemStateChanged;

    virtual void Tick(float DeltaTime) override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* ItemMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UBoxComponent* BoxCollision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USphereComponent* AreaCollision;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    UWidgetComponent* ItemInfoWidgetComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
    FName ItemName{"BaseItem"};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
    int32 ItemCount{0};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
    EItemRarity ItemRarity{EItemRarity::EIR_Damaged};

    EItemState CurrentItemState{EItemState::EIS_Pickup};

    UPROPERTY()
    TMap<EItemState, FItemProperties> ItemPropertiesMap;

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
};
