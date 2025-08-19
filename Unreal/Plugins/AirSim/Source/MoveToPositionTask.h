#pragma once

#include "CoreMinimal.h"
#include "LatentActions.h"
#include "MoveToPositionWorker.h"
#include "AirBlueprintLib.h"

class FMoveToPositionTask : public FPendingLatentAction
{
public:
    FName ExecutionFunction;
    int32 OutputLink;
    FWeakObjectPtr CallbackTarget;

    EBlueprintResult& Result;
    FMoveToPositionWorker* Worker;

    FMoveToPositionTask(const FLatentActionInfo& LatentInfo, const FVector& position, float velocity, float timeout_sec, const FString& vehicle_name, EBlueprintResult& result, float world_to_meters)
        : ExecutionFunction(LatentInfo.ExecutionFunction)
        , OutputLink(LatentInfo.Linkage)
        , CallbackTarget(LatentInfo.CallbackTarget)
        , Result(result)
        , Worker(new FMoveToPositionWorker(UAirBlueprintLib::GetClient(), position, velocity, timeout_sec, vehicle_name, world_to_meters))
    {
    }

    virtual ~FMoveToPositionTask()
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
        return TEXT("MoveToPosition");
    }
#endif
};
