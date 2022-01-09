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

DEFINE_LOG_CATEGORY_STATIC(LogWFBaseCharacter, All, All)

constexpr static float DeltaTimeMultiplier = 4.0f;

AWFBaseCharacter::AWFBaseCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->SetCapsuleSize(42.0f, 95.0f);

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetRootComponent());
    CameraBoom->TargetArmLength = 400.0f;
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->SocketOffset = FVector{0.0f, 110.0f, 120.0f};

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
}

void AWFBaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    checkf(GetCharacterMovement(), TEXT("CharacterMovementComponent = nullptr"));

    if (FollowCamera)
    {
        DefaultAngleFOV = GetFollowCamera()->FieldOfView;
        CurrentAngleFOV = GetFollowCamera()->FieldOfView;
    }
}

void AWFBaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdateZoomInterp(DeltaTime);
    UpdateCrossHairSpread(DeltaTime);
}

void AWFBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    checkf(PlayerInputComponent, TEXT("PlayerInputComponent = nullptr"));

    PlayerInputComponent->BindAxis("MoveForward", this, &AWFBaseCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AWFBaseCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &AWFBaseCharacter::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &AWFBaseCharacter::AddControllerPitchInput);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AWFBaseCharacter::FireWeapon);

    DECLARE_DELEGATE_OneParam(FOnZoomSignature, bool);
    PlayerInputComponent->BindAction<FOnZoomSignature>("Zoom", IE_Pressed, this, &AWFBaseCharacter::Zoom, true);
    PlayerInputComponent->BindAction<FOnZoomSignature>("Zoom", IE_Released, this, &AWFBaseCharacter::Zoom, false);
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
    Super::AddControllerYawInput(GetCurrentMouseSensitivity(Val));
}

void AWFBaseCharacter::AddControllerPitchInput(float Val)
{
    Super::AddControllerPitchInput(GetCurrentMouseSensitivity(Val));
}

float AWFBaseCharacter::GetCurrentMouseSensitivity(const float DefaultMouseSenseVal) const
{
    if (bIsAiming)
    {
        return ZoomMouseSensitivity * DefaultMouseSenseVal;
    }
    return DefaultMouseSenseVal * DefaultMouseSensitivity;
}

void AWFBaseCharacter::FireWeapon()
{
    InitFX();
    MakeShot();
}

void AWFBaseCharacter::MakeShot()
{
    if (!GetWorld()) return;

    FVector TraceStart, TraceEnd;
    if (!GetTraceData(TraceStart, TraceEnd)) return;

    //
    FVector TraceFXEnd{TraceEnd};
    //
    // Hit from ViewPoint
    FHitResult HitResult;
    MakeHit(HitResult, TraceStart, TraceEnd, TraceFXEnd);

    // Hit from WeaponMuzzleSocket
    FHitResult WeaponHitResult;
    FVector WeaponTraceStart{GetSocketLocation()}, WeaponTraceEnd{TraceFXEnd};
    MakeWeaponHit(WeaponHitResult, WeaponTraceStart, WeaponTraceEnd, TraceFXEnd);

    // Spawn FX
    SpawnImpactFX(TraceFXEnd);
    SpawnTraceFX(GetSocketLocation(), TraceFXEnd);

    // Play recoil animation
    PlayFireRecoilAnimMon();

    // Shoot CrossHair Spread
    StartCrossHairShoot();
}

bool AWFBaseCharacter::GetTraceData(FVector& TraceStart, FVector& TraceEnd) const
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
    if (HitResult.bBlockingHit)
    {
        TraceFXEnd = HitResult.ImpactPoint;
        bIsHit = true;
    }
}

void AWFBaseCharacter::MakeWeaponHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd, FVector& TraceFXEnd)
{
    if (!GetWorld()) return;

    GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);
    if (HitResult.bBlockingHit)
    {
        TraceFXEnd = HitResult.ImpactPoint;
        bIsHit = true;
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
    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFX, GetSocketLocation());

    // Sound
    UGameplayStatics::SpawnSoundAtLocation(GetWorld(), FireSound, GetSocketLocation());
}

void AWFBaseCharacter::SpawnImpactFX(const FVector& TraceFXEnd) const
{
    bIsHit
        ? UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactFX, TraceFXEnd)
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

void AWFBaseCharacter::Zoom(const bool bEnable)
{
    bIsAiming = bEnable;
    /*const auto PlayerController = Cast<APlayerController>(GetController());
    if (!PlayerController || !PlayerController->PlayerCameraManager) return;

    if (bEnable)
    {
        DefaultAngleFOV = PlayerController->PlayerCameraManager->GetFOVAngle();
    }
    PlayerController->PlayerCameraManager->SetFOV(bEnable ? ZoomAngleFOV : DefaultAngleFOV);*/
}

void AWFBaseCharacter::UpdateZoomInterp(const float DeltaTime)
{
    if (bIsAiming)
    {
        CurrentAngleFOV = FMath::FInterpTo(CurrentAngleFOV, ZoomAngleFOV, DeltaTime, ZoomInterpSpeed);
    }
    else
    {
        CurrentAngleFOV = FMath::FInterpTo(CurrentAngleFOV, DefaultAngleFOV, DeltaTime, ZoomInterpSpeed);
    }
    GetFollowCamera()->SetFieldOfView(CurrentAngleFOV);
}

void AWFBaseCharacter::UpdateCrossHairSpread(const float DeltaTime)
{
    // Velocity Spread
    const auto EditCrossHairVelocitySpread{UpdateCrossHairVelocitySpread()};

    // In air Spread
    UpdateInterpCrossHairSpread(GetCharacterMovement()->IsFalling(), EditCrossHairInAirSpread, CrossHairInAirSpread, DeltaTime, 1.0f);

    // Aim Spread
    UpdateInterpCrossHairSpread(bIsAiming, EditCrossHairAimSpread, CrossHairAimSpread, DeltaTime);

    // Shoot Spread
    UpdateInterpCrossHairSpread(bIsShootingBullet, EditCrossHairShootingSpread, CrossHairShootingSpread, DeltaTime, 60.0f, 60.0f);

    // Total Current Spread
    CurrentCrossHairSpread = FMath::Clamp
        (
            DefaultCrossHairSpread + EditCrossHairVelocitySpread + EditCrossHairInAirSpread - EditCrossHairAimSpread +
            EditCrossHairShootingSpread, //
            //
            CrossHairSpreadRange.X, //
            CrossHairSpreadRange.Y  //
            );
    //
}

float AWFBaseCharacter::UpdateCrossHairVelocitySpread() const
{
    const FVector2D Velocity2D{0.0f, GetCharacterMovement()->GetMaxSpeed()};
    const FVector2D Spread2D{0.0f, CrossHairVelocitySpread};
    FVector Velocity = GetVelocity();
    Velocity.Z = 0.0f;

    return FMath::GetMappedRangeValueClamped(Velocity2D, Spread2D, Velocity.Size());
}

void AWFBaseCharacter::UpdateInterpCrossHairSpread(const bool bIsEnable, float& EditCrossHairSpread, const float CrossHairSpread,
    const float DeltaTime, const float InterpSpeedEnable, const float InterpSpeedNOTEnable)
{
    if (bIsEnable)
    {
        EditCrossHairSpread = FMath::FInterpTo(EditCrossHairSpread, CrossHairSpread, DeltaTime, InterpSpeedEnable);
    }
    else
    {
        EditCrossHairSpread = FMath::FInterpTo(EditCrossHairSpread, 0.0f, DeltaTime, InterpSpeedNOTEnable);
    }
}

void AWFBaseCharacter::StartCrossHairShoot()
{
    bIsShootingBullet = true;
    GetWorldTimerManager().SetTimer(CrossHairShootTimerHandle, this, &AWFBaseCharacter::StopCrossHairShoot, ShootingTimeDuration, false);
}

void AWFBaseCharacter::StopCrossHairShoot()
{
    bIsShootingBullet = false;
}
