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
};