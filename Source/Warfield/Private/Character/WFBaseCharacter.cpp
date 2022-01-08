// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/WFBaseCharacter.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"

DEFINE_LOG_CATEGORY_STATIC(LogWFBaseCharacter, All, All)

AWFBaseCharacter::AWFBaseCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetRootComponent());
    CameraBoom->TargetArmLength = 400.0f;
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->SocketOffset = FVector{0.0f, 100.0f, 100.0f};

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, CameraBoom->SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    GetMesh()->SetRelativeLocation(FVector{0.0f, 0.0f, -88.0f});
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
}

void AWFBaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AWFBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    checkf(PlayerInputComponent, TEXT("PlayerInputComponent = nullptr"));

    PlayerInputComponent->BindAxis("MoveForward", this, &AWFBaseCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AWFBaseCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AWFBaseCharacter::FireWeapon);
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

void AWFBaseCharacter::FireWeapon()
{
    /*UNiagaraFunctionLibrary::SpawnSystemAttached
        (
            MuzzleFX,                      //
            GetMesh(),                     //
            WeaponMuzzleFXSocketName,      //
            FVector::ZeroVector,           //
            FRotator::ZeroRotator,         //
            EAttachLocation::SnapToTarget, //
            false                          //
            );*/

    const auto BarrelSocket = GetMesh()->GetSocketByName(WeaponMuzzleFXSocketName);
    if (BarrelSocket)
    {
        const auto SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFX, SocketTransform);

        FHitResult HitResult;
        FVector TraceStart, TraceEnd;
        MakeShot(HitResult, SocketTransform, TraceStart, TraceEnd);

        FVector TraceFXEnd{TraceEnd};

        if (HitResult.bBlockingHit)
        {
            TraceFXEnd = HitResult.ImpactPoint;
            /*DrawDebugLine(GetWorld(), TraceStart, HitResult.ImpactPoint, FColor::Red, false, 2.0f);
            DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 15.0f, FColor::Green, false, 2.0f);*/
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactFX, HitResult.ImpactPoint);
            /*}
            else
            {
                DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 2.0f);
            }*/
        }
        
        SpawnTraceFX(TraceStart, TraceFXEnd);
        
        UGameplayStatics::SpawnSoundAtLocation(GetWorld(), FireSound,
            GetMesh()->GetSocketLocation(WeaponMuzzleFXSocketName));

        const auto AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance && FireRecoilAnimMontage)
        {
            AnimInstance->Montage_Play(FireRecoilAnimMontage);
            AnimInstance->Montage_JumpToSection(FName("StartFire")); // Name of Section in AnimMontage
        }
    }
}

void AWFBaseCharacter::MakeShot(FHitResult& HitResult, const FTransform& SocketTransform, FVector& TraceStart,
    FVector& TraceEnd) const
{
    const auto Rotation{SocketTransform.GetRotation()};
    const auto RotationAxisX{Rotation.GetAxisX()};

    TraceStart = SocketTransform.GetLocation();
    TraceEnd = TraceStart + RotationAxisX * TraceDistance;

    GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);
}

void AWFBaseCharacter::SpawnTraceFX(const FVector& TraceFXStart, const FVector& TraceFXEnd)
{
    const auto TraceFXComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TraceFX, TraceFXStart);
    if (TraceFXComponent)
    {
        TraceFXComponent->SetVectorParameter(FName("Target"), TraceFXEnd);
    }
}
