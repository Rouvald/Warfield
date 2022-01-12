// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/WFBaseCharacter.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Sound/SoundCue.h"
#include "Components/WidgetComponent.h"

#include "Components/WFCrossHairComponent.h"
#include "WFBaseItem.h"


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

    PlayerInputComponent->BindAxis("MoveForward", this, &AWFBaseCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AWFBaseCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &AWFBaseCharacter::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &AWFBaseCharacter::AddControllerPitchInput);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AWFBaseCharacter::StartFire);
    PlayerInputComponent->BindAction("Fire", IE_Released, this, &AWFBaseCharacter::StopFire);

    DECLARE_DELEGATE_OneParam(FOnZoomSignature, bool);
    PlayerInputComponent->BindAction<FOnZoomSignature>("Zoom", IE_Pressed, CrossHairComponent, &UWFCrossHairComponent::Zoom, true);
    PlayerInputComponent->BindAction<FOnZoomSignature>("Zoom", IE_Released, CrossHairComponent, &UWFCrossHairComponent::Zoom, false);
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

/*
 *  TODO: Remove all Shooting logic into WeaponComponent and BaseWeapon
 */
void AWFBaseCharacter::StartFire()
{
    bIsButtonFirePressed = true;
    StartFireTimer();
}

void AWFBaseCharacter::StartFireTimer()
{
    if (!bCanFire) return;

    bCanFire = false;
    GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &AWFBaseCharacter::ResetFireTimer, ShootTimeRate);
    MakeShot();
}

void AWFBaseCharacter::ResetFireTimer()
{
    bCanFire = true;
    if (bIsButtonFirePressed)
    {
        StartFireTimer();
    }
}

void AWFBaseCharacter::StopFire()
{
    bIsButtonFirePressed = false;
}

void AWFBaseCharacter::MakeShot()
{
    if (!GetWorld()) return;

    FVector TraceStart, TraceEnd;
    if (!GetTraceData(TraceStart, TraceEnd, ShootTraceDistance)) return;

    InitFX();

    // Bullet Trace End FVector
    FVector TraceFXEnd{TraceEnd};
    //
    // Hit from ViewPoint
    FHitResult DefaultHitResult;
    MakeHit(DefaultHitResult, TraceStart, TraceEnd, TraceFXEnd);

    // Hit from WeaponMuzzleSocket
    FHitResult WeaponHitResult;
    FVector WeaponTraceStart{GetSocketLocation()};
    /*
     * @note: Sometimes Trace can't reach DefaultHitResult.ImpactPoint
     *        This 2 lines fix this error
     */
    const FVector StartToEndTraces{TraceFXEnd - GetSocketLocation()};
    FVector WeaponTraceEnd{GetSocketLocation() + StartToEndTraces * 1.1f};
    //
    /* MakeHit for second trace, from weapon muzzle socket */
    MakeHit(WeaponHitResult, WeaponTraceStart, WeaponTraceEnd, TraceFXEnd);

    //DrawDebugSphere(GetWorld(), WeaponHitResult.ImpactPoint, 10.0f, 16, FColor::Green, false, 10.0f);

    /*
     * @note We have 2 FHitResult: DefaultHitResult and WeaponHitResult.
     *       It's needed for correct build trace from WeaponMuzzle and HitPoint (if have HitPoint).
     *       For Correct working FX we need use WeaponHitResult. Because If DefaultHitResult hit someone,
     *       WeaponHitResult anyways hit same Object and will have same ImpactPoint.
     *
     *       Another situation:  Nothing had been hit 
     *       In this case for spawning FX will be used TraceFXEnd.
     */

    // Spawn FX
    SpawnImpactFX(WeaponHitResult, TraceFXEnd);
    SpawnTraceFX(GetSocketLocation(), TraceFXEnd);
    //

    // Play recoil animation
    PlayFireRecoilAnimMon();

    // Shoot CrossHair Spread
    CrossHairComponent->StartCrossHairShoot();
}

bool AWFBaseCharacter::GetTraceData(FVector& TraceStart, FVector& TraceEnd, const float TraceDistance) const
{
    FVector ViewLocation;
    FRotator ViewRotation;

    if (!Controller) return false;

    Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);

    TraceStart = ViewLocation;
    const auto ShootDirection = ViewRotation.Vector();
    TraceEnd = TraceStart + ShootDirection * TraceDistance;
    return true;
}

void AWFBaseCharacter::MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd, FVector& TraceFXEnd)
{
    if (!GetWorld()) return;

    GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);
    //DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 10.0f);
    if (HitResult.bBlockingHit)
    {
        TraceFXEnd = HitResult.ImpactPoint;
        bIsHit = true;
    }
    else
    {
        bIsHit = false;
    }
}

void AWFBaseCharacter::PlayFireRecoilAnimMon() const
{
    const auto AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && FireRecoilAnimMontage)
    {
        AnimInstance->Montage_Play(FireRecoilAnimMontage);
        AnimInstance->Montage_JumpToSection(FName("StartFire")); // Name of Section in AnimMontage
    }
}

void AWFBaseCharacter::InitFX() const
{
    // Muzzle FX
    /* TODO: In future remade with SpawnAtLocation*/
    UGameplayStatics::SpawnEmitterAttached
        (
            MuzzleFX,                      //
            GetMesh(),                     //
            WeaponMuzzleFXSocketName,      //
            FVector::ZeroVector,           //
            FRotator::ZeroRotator,         //
            EAttachLocation::SnapToTarget, //
            true                           //
            );
    //UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFX, GetSocketLocation());

    // Sound
    UGameplayStatics::SpawnSoundAtLocation(GetWorld(), FireSound, GetSocketLocation());
}

void AWFBaseCharacter::SpawnImpactFX(const FHitResult& HitResult, const FVector& TraceFXEnd) const
{
    // Impact FX
    bIsHit
        ? UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactFX, HitResult.ImpactPoint, HitResult.Normal.Rotation())
        : UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), NoHitImpactFX, TraceFXEnd);
}

void AWFBaseCharacter::SpawnTraceFX(const FVector& TraceFXStart, const FVector& TraceFXEnd) const
{
    // Trace FX
    const auto TraceFXComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TraceFX, TraceFXStart);
    if (TraceFXComponent)
    {
        TraceFXComponent->SetVectorParameter(FName("Target"), TraceFXEnd);
    }
}

FVector AWFBaseCharacter::GetSocketLocation() const
{
    return GetMesh()->GetSocketLocation(WeaponMuzzleFXSocketName);
}

void AWFBaseCharacter::ItemInfoVisibilityTimer(const AWFBaseItem* Item, bool bIsOverlap)
{
    const FTimerDelegate ItemTimerDelegate = FTimerDelegate::CreateUObject(this, &AWFBaseCharacter::UpdateItemInfoVisibility);
    if (bIsOverlap)
    {
        GetWorldTimerManager().SetTimer(ItemInfoVisibilityTimerHandle, ItemTimerDelegate, 0.1f, true);
        HittedItems.Add(Item);
    }
    else
    {
        HittedItems.RemoveSingle(Item);
        if (HittedItems.Num() == 0)
        {
            GetWorldTimerManager().ClearTimer(ItemInfoVisibilityTimerHandle);
        }
    }
    //UE_LOG(LogWFBaseCharacter, Warning, TEXT("HittedItems: %d"), HittedItems.Num());
}

void AWFBaseCharacter::UpdateItemInfoVisibility()
{
    FHitResult HitItemResult;

    if (!MakeHitItemVisibility(HitItemResult)) return;

    const auto HitItem = Cast<AWFBaseItem>(HitItemResult.GetActor());
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
    if (!GetTraceData(TraceStart, TraceEnd, ItemVisibilityTraceDistance)) return false;

    FCollisionQueryParams CollisionQueryParams;
    CollisionQueryParams.AddIgnoredActor(this);

    GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionQueryParams);
    //DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.0f);

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
