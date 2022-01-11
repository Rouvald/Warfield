// Warfield Game. All Rigths Reserved


#include "Components/WFWeaponComponent.h"

UWFWeaponComponent::UWFWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UWFWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
}
