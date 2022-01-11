// Warfield Game. All Rigths Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WFWeaponComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WARFIELD_API UWFWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWFWeaponComponent();

protected:
	virtual void BeginPlay() override;
};
