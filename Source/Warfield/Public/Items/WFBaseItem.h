// Warfield Game. All Rigths Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WFBaseItem.generated.h"

class UBoxComponent;

UCLASS()
class WARFIELD_API AWFBaseItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AWFBaseItem();

    virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
    USkeletalMeshComponent* ItemMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
    UBoxComponent* BoxCollision;

};
