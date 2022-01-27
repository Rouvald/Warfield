// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/WFBaseCharacter.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"

#include "Components/WFCrossHairComponent.h"
#include "Components/WFWeaponComponent.h"
#include "WFBaseItem.h"
#include "WFBaseWeapon.h"
#include "WFUtils.h"


DEFINE_LOG_CATEGORY_STATIC(LogWFBaseCharacter, All, All)

AWFBaseCharacter::AWFBaseCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->SetCapsuleSize(42.0f, 95.0f);

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetRootComponent());
    CameraBoom->TargetArmLength = 500.0f;
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->SocketOffset = FVector{0.0f, 110.0f, 140.0f};

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, CameraBoom->SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    GetMesh()->SetRelativeLocation(FVector{0.0f, 0.0f, -97.0f});
    GetMesh()->SetRelativeRotation(FRotator{0.0f, -90.0f, 0.0f});

    //
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;
    //
    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->RotationRate = FRotator{0.0f, 540.0f, 0.0f};
    GetCharacterMovement()->JumpZVelocity = 550.0f;
    GetCharacterMovement()->AirControl = 0.2f;
    //

    CrossHairComponent = CreateDefaultSubobject<UWFCrossHairComponent>(TEXT("CrossHairComponent"));
    WeaponComponent = CreateDefaultSubobject<UWFWeaponComponent>(TEXT("WeaponComponent"));
}

void AWFBaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    checkf(GetCharacterMovement(), TEXT("CharacterMovementComponent = nullptr"));

    OnItemAreaOverlap.AddUObject(this, &AWFBaseCharacter::ItemInfoVisibilityTimer);
}

void AWFBaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AWFBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    checkf(PlayerInputComponent, TEXT("PlayerInputComponent = nullptr"));
    checkf(CrossHairComponent, TEXT("CrossHairComponent = nullptr"));
    checkf(WeaponComponent, TEXT("WeaponComponent = nullptr"));

    PlayerInputComponent->BindAxis("MoveForward", this, &AWFBaseCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AWFBaseCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &AWFBaseCharacter::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &AWFBaseCharacter::AddControllerPitchInput);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    PlayerInputComponent->BindAction("Fire", IE_Pressed, WeaponComponent, &UWFWeaponComponent::StartFire);
    PlayerInputComponent->BindAction("Fire", IE_Released, WeaponComponent, &UWFWeaponComponent::StopFire);

    DECLARE_DELEGATE_OneParam(FOnZoomSignature, bool);
    PlayerInputComponent->BindAction<FOnZoomSignature>("Zoom", IE_Pressed, CrossHairComponent, &UWFCrossHairComponent::Zoom, true);
    PlayerInputComponent->BindAction<FOnZoomSignature>("Zoom", IE_Released, CrossHairComponent, &UWFCrossHairComponent::Zoom, false);

    PlayerInputComponent->BindAction("TakeWeapon", IE_Pressed, WeaponComponent, &UWFWeaponComponent::TakeWeaponButtonPressed);
    PlayerInputComponent->BindAction("TakeWeapon", IE_Released, WeaponComponent, &UWFWeaponComponent::TakeWeaponButtonReleased);

    PlayerInputComponent->BindAction("DropWeapon", IE_Pressed, WeaponComponent, &UWFWeaponComponent::DropWeaponButtonPressed);
    PlayerInputComponent->BindAction("DropWeapon", IE_Released, WeaponComponent, &UWFWeaponComponent::DropWeaponButtonReleased);
}

void AWFBaseCharacter::MoveForward(const float Value)
{
    if (!Controller || FMath::IsNearlyZero(Value)) return;

    const auto Rotation{Controller->GetControlRotation()};
    const auto YawRotation{FRotator{0.0f, Rotation.Yaw, 0.0f}};

    const auto Direction{FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X)};
    AddMovementInput(Direction, Value);
}

void AWFBaseCharacter::MoveRight(const float Value)
{
    if (!Controller || FMath::IsNearlyZero(Value)) return;

    const auto Rotation{Controller->GetControlRotation()};
    const auto YawRotation{FRotator{0.0f, Rotation.Yaw, 0.0f}};

    const auto Direction{FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y)};
    AddMovementInput(Direction, Value);
}

void AWFBaseCharacter::AddControllerYawInput(float Val)
{
    Super::AddControllerYawInput(CrossHairComponent->GetCurrentMouseSensitivity(Val));
}

void AWFBaseCharacter::AddControllerPitchInput(float Val)
{
    Super::AddControllerPitchInput(CrossHairComponent->GetCurrentMouseSensitivity(Val));
}

void AWFBaseCharacter::ItemInfoVisibilityTimer(const AWFBaseItem* Item, bool bIsOverlap)
{
    const FTimerDelegate ItemTimerDelegate = FTimerDelegate::CreateUObject(this, &AWFBaseCharacter::UpdateItemInfoVisibility);
    if (bIsOverlap)
    {
        GetWorldTimerManager().SetTimer(ItemInfoVisibilityTimerHandle, ItemTimerDelegate, 0.1f, true);
        HittedItems.AddUnique(Item);
    }
    else
    {
        HittedItems.RemoveSingle(Item);
        if (HittedItems.Num() == 0)
        {
            GetWorldTimerManager().ClearTimer(ItemInfoVisibilityTimerHandle);
        }
    }
    //UE_LOG(LogWFBaseCharacter, Display, TEXT("HittedItems: %d"), HittedItems.Num());
}

void AWFBaseCharacter::UpdateItemInfoVisibility()
{
    FHitResult HitItemResult;

    if (!MakeHitItemVisibility(HitItemResult)) return;

    HitItem = Cast<AWFBaseItem>(HitItemResult.GetActor());
    if (!HitItem)
    {
        HideAllHittedItems();
        return;
    }

    const auto bIsContains = HittedItems.Contains(HitItem);

    const auto ItemInfo = HitItem->FindComponentByClass<UWidgetComponent>();
    if (ItemInfo)
    {
        bIsContains ? ItemInfo->SetVisibility(true) : ItemInfo->SetVisibility(false);
    }
}

bool AWFBaseCharacter::MakeHitItemVisibility(FHitResult& HitResult)
{
    if (!GetWorld()) return false;

    FVector TraceStart, TraceEnd;
    if (!WFUtils::GetTraceData(this, TraceStart, TraceEnd, ItemVisibilityTraceDistance)) return false;

    FCollisionQueryParams CollisionQueryParams;
    CollisionQueryParams.AddIgnoredActor(this);

    GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionQueryParams);
    // DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.0f);

    return HitResult.bBlockingHit;
}

void AWFBaseCharacter::HideAllHittedItems() const
{
    for (const auto HittedItem : HittedItems)
    {
        const auto ItemInfo = HittedItem->FindComponentByClass<UWidgetComponent>();
        if (ItemInfo)
        {
            ItemInfo->SetVisibility(false);
        }
    }
}

FVector AWFBaseCharacter::GetCameraInterpLocation() const
{
    return FollowCamera->GetComponentLocation() + FollowCamera->GetForwardVector() * CameraInterpDistance +
           FVector{0.0f, 0.0f, CameraInterpElevation};
}

