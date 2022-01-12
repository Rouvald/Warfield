// Warfield Game. All Rigths Reserved


#include "Items/WFBaseItem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"

AWFBaseItem::AWFBaseItem()
{
	PrimaryActorTick.bCanEverTick = true;

    ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
    SetRootComponent(ItemMesh);

    BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
    BoxCollision->SetupAttachment(GetRootComponent());

    ItemInfoWidget=CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemInfoWidget"));
    ItemInfoWidget->SetupAttachment(GetRootComponent());
    ItemInfoWidget->SetWidgetSpace(EWidgetSpace::Screen);
    ItemInfoWidget->SetDrawAtDesiredSize(true);
}

void AWFBaseItem::BeginPlay()
{
	Super::BeginPlay();
	
}
void AWFBaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

