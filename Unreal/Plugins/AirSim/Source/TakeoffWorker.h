#pragma once

#include "HAL/Runnable.h"
#include "HAL/PlatformProcess.h"
#include "HAL/RunnableThread.h"

// We forward-declare these to avoid including the heavy headers here.
namespace msr { namespace airlib { class MultirotorRpcLibClient; } }
namespace rpc { class rpc_error; }

class FTakeoffWorker : public FRunnable
{
public:
    FTakeoffWorker(msr::airlib::MultirotorRpcLibClient* client, float timeout_sec, const FString& vehicle_name);
    ~FTakeoffWorker();

    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;

    bool IsFinished() const;
    bool WasSuccessful() const;

private:
    // This order must match the order in the constructor's initializer list to avoid warnings.
    msr::airlib::MultirotorRpcLibClient* Client;
    float Timeout;
    FString VehicleName;
    FRunnableThread* Thread;
    std::atomic<bool> bIsFinished;
    std::atomic<bool> bSuccess;
};
