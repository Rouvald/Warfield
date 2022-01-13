// Warfield Game. All Rights Reserved

#include "Items/WFBaseItem.h"

#include "WFBaseCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/WFItemInfoWidget.h"

AWFBaseItem::AWFBaseItem()
{
    PrimaryActorTick.bCanEverTick = true;

    ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
    SetRootComponent(ItemMesh);

    BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
    BoxCollision->SetupAttachment(GetRootComponent());
    BoxCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    BoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

    AreaComponent = CreateDefaultSubobject<USphereComponent>(TEXT("AreaComponent"));
    AreaComponent->SetupAttachment(GetRootComponent());

    ItemInfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemInfoWidgetComponent"));
    ItemInfoWidgetComponent->SetupAttachment(GetRootComponent());
    ItemInfoWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    ItemInfoWidgetComponent->SetDrawAtDesiredSize(true);
    ItemInfoWidgetComponent->SetRelativeLocation(FVector{0.0f, 0.0f, 120.0f});
}

void AWFBaseItem::BeginPlay()
{
    Super::BeginPlay();
    ItemInfoWidgetComponent->SetVisibility(false);

    AreaComponent->OnComponentBeginOverlap.AddDynamic(this, &AWFBaseItem::OnAreaBeginOverlap);
    AreaComponent->OnComponentEndOverlap.AddDynamic(this, &AWFBaseItem::OnAreaEndOverlap);

    SetItemInfo();
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
}

void AWFBaseItem::SetItemRarity(const UWFItemInfoWidget* ItemInfoWidget) const
{
    // TODO: Improve it. (maybe it can't be improved) :(
    /* Set number of Visible stars */
    switch (ItemRarity)
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
    }
}
