#pragma once

#include "CoreMinimal.h"
#include "LatentActions.h"
#include "RotateToYawWorker.h"
#include "AirBlueprintLib.h"

class FRotateToYawTask : public FPendingLatentAction
{
public:
    FName ExecutionFunction;
    int32 OutputLink;
    FWeakObjectPtr CallbackTarget;

    EBlueprintResult& Result;
    FRotateToYawWorker* Worker;

    FRotateToYawTask(const FLatentActionInfo& LatentInfo, float yaw, float timeout_sec, const FString& vehicle_name, EBlueprintResult& result)
        : ExecutionFunction(LatentInfo.ExecutionFunction)
        , OutputLink(LatentInfo.Linkage)
        , CallbackTarget(LatentInfo.CallbackTarget)
        , Result(result)
        , Worker(new FRotateToYawWorker(UAirBlueprintLib::GetClient(), yaw, timeout_sec, vehicle_name))
    {
    }

    virtual ~FRotateToYawTask()
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
        return TEXT("RotateToYaw");
    }
#endif
};
