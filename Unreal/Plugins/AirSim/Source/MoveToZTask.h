#pragma once

#include "CoreMinimal.h"
#include "LatentActions.h"
#include "MoveToZWorker.h"
#include "AirBlueprintLib.h"

class FMoveToZTask : public FPendingLatentAction
{
public:
    FName ExecutionFunction;
    int32 OutputLink;
    FWeakObjectPtr CallbackTarget;

    EBlueprintResult& Result;
    FMoveToZWorker* Worker;

    FMoveToZTask(const FLatentActionInfo& LatentInfo, float z, float velocity, float timeout_sec, const FString& vehicle_name, EBlueprintResult& result, float world_to_meters)
        : ExecutionFunction(LatentInfo.ExecutionFunction)
        , OutputLink(LatentInfo.Linkage)
        , CallbackTarget(LatentInfo.CallbackTarget)
        , Result(result)
        , Worker(new FMoveToZWorker(UAirBlueprintLib::GetClient(), z, velocity, timeout_sec, vehicle_name, world_to_meters))
    {
    }

    virtual ~FMoveToZTask()
    {
        delete Worker;
    }

    virtual void UpdateOperation(FLatentResponse& Response) override
    {
        if (Worker->IsFinished())
        {
            Result = Worker->WasSuccessful() ? EBlueprintResult::Success : EBlueprintResult::Failure;
            Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
        }
    }

#if WITH_EDITOR
    virtual FString GetDescription() const override
    {
        return TEXT("MoveToZ");
    }
#endif
};
