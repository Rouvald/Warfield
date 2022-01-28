#pragma once

class WFUtils
{
public:
    
    static bool GetTraceData(APawn* PlayerPawn, FVector& TraceStart, FVector& TraceEnd, const float TraceDistance)
    {
        if(!PlayerPawn) return false;

        if (!PlayerPawn->GetController()) return false;

        FVector ViewLocation;
        FRotator ViewRotation;
    
        PlayerPawn->GetController()->GetPlayerViewPoint(ViewLocation, ViewRotation);

        TraceStart = ViewLocation;
        const auto ShootDirection = ViewRotation.Vector();
        TraceEnd = TraceStart + ShootDirection * TraceDistance;
        return true;
    }

    static FText TextFromInt(const int32 Number)
    {
        return FText::FromString(FString::FromInt(Number));
    }

    static void PlayAnimMontage(AWFBaseCharacter* Character, UAnimMontage* AnimMontage, const FName SectionName)
    {
        if (!Character) return;

        const auto AnimInstance = Character->GetMesh()->GetAnimInstance();
        if (!AnimMontage || !AnimInstance) return;

        // @todo: Create WeaponType and change NameSection
        // @todo: Create Struct for weapon Type and another specific type properties

        AnimInstance->Montage_Play(AnimMontage);
        AnimInstance->Montage_JumpToSection(SectionName);
    }
};