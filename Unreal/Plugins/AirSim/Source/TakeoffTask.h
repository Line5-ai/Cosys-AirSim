#pragma once

#include "CoreMinimal.h"
#include "LatentActions.h"
#include "TakeoffWorker.h"
#include "AirBlueprintLib.h"

class FTakeoffTask : public FPendingLatentAction
{
public:
    FName ExecutionFunction;
    int32 OutputLink;
    FWeakObjectPtr CallbackTarget;

    EBlueprintResult& Result;
    FTakeoffWorker* Worker;

    FTakeoffTask(const FLatentActionInfo& LatentInfo, float timeout_sec, const FString& vehicle_name, EBlueprintResult& result)
        : ExecutionFunction(LatentInfo.ExecutionFunction)
        , OutputLink(LatentInfo.Linkage)
        , CallbackTarget(LatentInfo.CallbackTarget)
        , Result(result)
        , Worker(new FTakeoffWorker(UAirBlueprintLib::GetClient(), timeout_sec, vehicle_name))
    {
    }

    virtual ~FTakeoffTask()
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
        return TEXT("Takeoff");
    }
#endif
};
