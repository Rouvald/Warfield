// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/WFBaseCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
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

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, CameraBoom->SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    GetMesh()->SetRelativeLocation(FVector{0.0f, 0.0f, -88.0f});
    GetMesh()->SetRelativeRotation(FRotator{0.0f, -90.0f, 0.0f});

    //
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;
    //
    GetCharacterMovement()->bOrientRotationToMovement = true;
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
            WeaponMuzzFXleSocketName,      //
            FVector::ZeroVector,           //
            FRotator::ZeroRotator,         //
            EAttachLocation::SnapToTarget, //
            false                          //
            );*/

    const auto BarrelSocket = GetMesh()->GetSocketByName(WeaponMuzzFXleSocketName);
    if(BarrelSocket)
    {
        const auto SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFX, SocketTransform);
    }

    UGameplayStatics::SpawnSoundAttached(FireSound, GetMesh(), WeaponMuzzFXleSocketName);
}
