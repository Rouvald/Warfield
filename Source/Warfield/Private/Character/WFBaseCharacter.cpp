// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/WFBaseCharacter.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/WFCrossHairComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Sound/SoundCue.h"

DEFINE_LOG_CATEGORY_STATIC(LogWFBaseCharacter, All, All)

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

    CrossHairComponent = CreateDefaultSubobject<UWFCrossHairComponent>(TEXT("CrossHairComponent"));
}

void AWFBaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    checkf(GetCharacterMovement(), TEXT("CharacterMovementComponent = nullptr"));
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
    if (!GetTraceData(TraceStart, TraceEnd)) return;

    InitFX();

    // Bullet Trace End FVector
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
    CrossHairComponent->StartCrossHairShoot();
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
