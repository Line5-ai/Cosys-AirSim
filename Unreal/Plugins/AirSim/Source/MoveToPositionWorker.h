#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/PlatformProcess.h"
#include "HAL/RunnableThread.h"

// We forward-declare these to avoid including the heavy headers here.
namespace msr { namespace airlib { class MultirotorRpcLibClient; } }
namespace rpc { class rpc_error; }

class FMoveToPositionWorker : public FRunnable
{
public:
    FMoveToPositionWorker(msr::airlib::MultirotorRpcLibClient* client, const FVector& position, float velocity, float timeout_sec, const FString& vehicle_name, float world_to_meters);
    ~FMoveToPositionWorker();

    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;

    bool IsFinished() const;
    bool WasSuccessful() const;

private:
    // This order must match the order in the constructor's initializer list to avoid warnings.
    msr::airlib::MultirotorRpcLibClient* Client;
    FVector Position;
    float Velocity;
    float Timeout;
    FString VehicleName;
    float WorldToMetersScale;
    FRunnableThread* Thread;
    std::atomic<bool> bIsFinished;
    std::atomic<bool> bSuccess;
};
