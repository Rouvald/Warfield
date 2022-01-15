// Warfield Game. All Rights Reserved

#include "Items/WFBaseItem.h"

#include "WFBaseCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/WFItemInfoWidget.h"

DEFINE_LOG_CATEGORY_STATIC(LogWFBaseItem, All, All)

AWFBaseItem::AWFBaseItem()
{
    PrimaryActorTick.bCanEverTick = true;

    ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
    SetRootComponent(ItemMesh);

    BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
    BoxCollision->SetupAttachment(GetRootComponent());
    BoxCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    BoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

    AreaCollision = CreateDefaultSubobject<USphereComponent>(TEXT("AreaCollision"));
    AreaCollision->SetupAttachment(GetRootComponent());
    AreaCollision->SetSphereRadius(500.0f);

    ItemInfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemInfoWidgetComponent"));
    ItemInfoWidgetComponent->SetupAttachment(GetRootComponent());
    ItemInfoWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    ItemInfoWidgetComponent->SetDrawAtDesiredSize(true);
    ItemInfoWidgetComponent->SetRelativeLocation(FVector{0.0f, 0.0f, 120.0f});
}

void AWFBaseItem::BeginPlay()
{
    Super::BeginPlay();

    checkf(ItemMesh, TEXT("ItemMesh = nullptr"));
    checkf(BoxCollision, TEXT("BoxCollision = nullptr"));
    checkf(AreaCollision, TEXT("AreaCollision = nullptr"));

    ItemInfoWidgetComponent->SetVisibility(false);

    AreaCollision->OnComponentBeginOverlap.AddDynamic(this, &AWFBaseItem::OnAreaBeginOverlap);
    AreaCollision->OnComponentEndOverlap.AddDynamic(this, &AWFBaseItem::OnAreaEndOverlap);
    OnItemStateChanged.AddUObject(this, &AWFBaseItem::SetItemState);

    FillItemPropertiesMap();
    SetItemInfo();
    //SetItemProperties(CurrentItemState);
}

void AWFBaseItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AWFBaseItem::OnAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor) return;

    const auto Character = Cast<AWFBaseCharacter>(OtherActor);
    if (!Character) return;

    Character->OnItemAreaOverlap.Broadcast(this, true);
}

void AWFBaseItem::OnAreaEndOverlap(
    UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!OtherActor) return;

    const auto Character = Cast<AWFBaseCharacter>(OtherActor);
    if (!Character) return;

    Character->OnItemAreaOverlap.Broadcast(this, false);
    ItemInfoWidgetComponent->SetVisibility(false);
}

void AWFBaseItem::SetItemInfo() const
{
    const auto ItemInfoWidget = Cast<UWFItemInfoWidget>(ItemInfoWidgetComponent->GetWidget());

    if (ItemInfoWidget)
    {
        ItemInfoWidget->SetItemName(ItemName);
        ItemInfoWidget->SetItemCount(ItemCount);
        SetItemRarity(ItemInfoWidget);
    }
    OnItemStateChanged.Broadcast(EItemState::EIS_Pickup);
}

void AWFBaseItem::SetItemRarity(const UWFItemInfoWidget* ItemInfoWidget) const
{
    /** @todo: Improve it: static_cast<int8> controversial decision */

    /* Set number of Visible stars */
    /*switch (ItemRarity)
    {
    case EItemRarity::EIR_Damaged:
        ItemInfoWidget->SetItemRarityVisibility(1);
        break;
    case EItemRarity::EIR_Rare:
        ItemInfoWidget->SetItemRarityVisibility(2);
        break;
    case EItemRarity::EIR_Mythical:
        ItemInfoWidget->SetItemRarityVisibility(3);
        break;
    case EItemRarity::EIR_Legendary:
        ItemInfoWidget->SetItemRarityVisibility(4);
        break;
    case EItemRarity::EIR_Immortal:
        ItemInfoWidget->SetItemRarityVisibility(5);
        break;
    case EItemRarity::EIR_MAX:
        break;
    }*/
    ItemInfoWidget->SetItemRarityVisibility(static_cast<int8>(ItemRarity));
}

void AWFBaseItem::SetItemState(EItemState NewItemState)
{
    if (CurrentItemState == NewItemState) return;

    CurrentItemState = NewItemState;
    SetItemProperties(NewItemState);
    //UE_LOG(LogWFBaseItem, Display, TEXT("CurrentItemState: %s"), *UEnum::GetValueAsString(CurrentItemState));
}

void AWFBaseItem::FillItemPropertiesMap()
{
    /* @todo: Future refactoring */
    ItemPropertiesMap.Add(EItemState::EIS_Pickup, FItemProperties
    {false, false, true, ECollisionResponse::ECR_Ignore,
     ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Ignore,
     ECollisionEnabled::NoCollision,
     ECollisionResponse::ECR_Overlap, ECollisionEnabled::QueryOnly,
     ECollisionResponse::ECR_Ignore, ECollisionEnabled::QueryAndPhysics,
     ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block,
    });

    ItemPropertiesMap.Add(EItemState::EIS_Equipped, FItemProperties
    {false, false, true, ECollisionResponse::ECR_Ignore,
     ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Ignore,
     ECollisionEnabled::NoCollision,
     ECollisionResponse::ECR_Ignore, ECollisionEnabled::NoCollision,
     ECollisionResponse::ECR_Ignore, ECollisionEnabled::QueryAndPhysics,
     ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore
    });

    ItemPropertiesMap.Add(EItemState::EIS_Falling, FItemProperties
    {true, true, true, ECollisionResponse::ECR_Ignore,
     ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block,
     ECollisionEnabled::QueryAndPhysics,
     ECollisionResponse::ECR_Ignore, ECollisionEnabled::NoCollision,
     ECollisionResponse::ECR_Ignore, ECollisionEnabled::NoCollision,
     ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore
    });
}

void AWFBaseItem::SetItemProperties(EItemState NewItemState) const
{
    if (!ItemPropertiesMap.Contains(NewItemState)) return;

    // Item Mesh
    ItemMesh->SetSimulatePhysics(ItemPropertiesMap[NewItemState].bIsSimulatedPhysics);
    ItemMesh->SetEnableGravity(ItemPropertiesMap[NewItemState].bIsGravityEnable);
    ItemMesh->SetVisibility(ItemPropertiesMap[NewItemState].bIsVisible);
    ItemMesh->SetCollisionResponseToAllChannels(ItemPropertiesMap[NewItemState].ItemMeshCollisionResponseToAllChannels);
    ItemMesh->SetCollisionResponseToChannel(ItemPropertiesMap[NewItemState].ItemMeshCollisionChannel,
        ItemPropertiesMap[NewItemState].ItemMeshCollisionResponseToChannel);
    ItemMesh->SetCollisionEnabled(ItemPropertiesMap[NewItemState].ItemMeshCollisionEnabled);
    //
    // Area Collision
    AreaCollision->SetCollisionResponseToAllChannels(ItemPropertiesMap[NewItemState].AreaCollisionResponseToAllChannels);
    AreaCollision->SetCollisionEnabled(ItemPropertiesMap[NewItemState].AreaCollisionEnabled);
    //
    // Box Collision
    BoxCollision->SetCollisionResponseToAllChannels(ItemPropertiesMap[NewItemState].BoxCollisionResponseToAllChannels);
    BoxCollision->SetCollisionResponseToChannel(ItemPropertiesMap[NewItemState].BoxCollisionChannel,
        ItemPropertiesMap[NewItemState].BoxCollisionResponseToChannel);
    BoxCollision->SetCollisionEnabled(ItemPropertiesMap[NewItemState].BoxCollisionEnabled);
    //
}
