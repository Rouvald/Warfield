// Warfield Game. All Rights Reserved


#include "Items/WFBaseItem.h"

#include "WFBaseCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

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

    ItemInfoWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemInfoWidget"));
    ItemInfoWidget->SetupAttachment(GetRootComponent());
    ItemInfoWidget->SetWidgetSpace(EWidgetSpace::Screen);
    ItemInfoWidget->SetDrawAtDesiredSize(true);
    ItemInfoWidget->SetRelativeLocation(FVector{0.0f, 0.0f, 120.0f});
}

void AWFBaseItem::BeginPlay()
{
    Super::BeginPlay();
    ItemInfoWidget->SetVisibility(false);

    AreaComponent->OnComponentBeginOverlap.AddDynamic(this, &AWFBaseItem::OnAreaBeginOverlap);
    AreaComponent->OnComponentEndOverlap.AddDynamic(this, &AWFBaseItem::OnAreaEndOverlap);
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

void AWFBaseItem::OnAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    if (!OtherActor) return;
    
    const auto Character = Cast<AWFBaseCharacter>(OtherActor);
    if (!Character) return;

    Character->OnItemAreaOverlap.Broadcast(this, false);
    //ItemInfoWidget->SetVisibility(false);
}
