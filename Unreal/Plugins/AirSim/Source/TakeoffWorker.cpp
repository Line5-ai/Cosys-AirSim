#include "TakeoffWorker.h"
#include "AirBlueprintLib.h"
#include "common/Common.hpp"

//This is required for the rpclib to be included correctly
#include "common/common_utils/StrictMode.hpp"
STRICT_MODE_OFF
#ifndef RPCLIB_MSGPACK
#define RPCLIB_MSGPACK clmdep_msgpack
#endif // !RPCLIB_MSGPACK
#ifdef nil
#undef nil
#endif // nil
#include "common/common_utils/WindowsApisCommonPre.hpp"
#undef FLOAT
#undef check
#include "rpc/client.h"
#include "rpc/rpc_error.h"
#ifndef check
#define check(expr) (static_cast<void>((expr)))
#endif
#include "common/common_utils/WindowsApisCommonPost.hpp"
STRICT_MODE_ON

#include "vehicles/multirotor/api/MultirotorRpcLibClient.hpp"

FTakeoffWorker::FTakeoffWorker(msr::airlib::MultirotorRpcLibClient* client, float timeout_sec, const FString& vehicle_name)
    : Client(client)
    , Timeout(timeout_sec)
    , VehicleName(vehicle_name)
    , bIsFinished(false)
    , bSuccess(false)
{
    Thread = FRunnableThread::Create(this, TEXT("TakeoffWorkerThread"), 0, TPri_BelowNormal);
}

FTakeoffWorker::~FTakeoffWorker()
{
    if (Thread)
    {
        Thread->Kill(true);
        delete Thread;
    }
}

bool FTakeoffWorker::Init()
{
    return true;
}

uint32 FTakeoffWorker::Run()
{
    try
    {
        std::string vehicle_name_std = TCHAR_TO_UTF8(*VehicleName);
        UE_LOG(LogTemp, Log, TEXT("Takeoff Task: Connecting to drone '%s'..."), *VehicleName);

        msr::airlib::MultirotorRpcLibClient client("localhost", 41451, Timeout);
        client.confirmConnection();
        UE_LOG(LogTemp, Log, TEXT("Takeoff Task: Connection confirmed."));

        UE_LOG(LogTemp, Log, TEXT("Takeoff Task: Enabling API control..."));
        client.enableApiControl(true, vehicle_name_std);
        UE_LOG(LogTemp, Log, TEXT("Takeoff Task: API control enabled."));

        UE_LOG(LogTemp, Log, TEXT("Takeoff Task: Arming drone..."));
        client.armDisarm(true, vehicle_name_std);
        UE_LOG(LogTemp, Log, TEXT("Takeoff Task: Drone armed."));

        UE_LOG(LogTemp, Log, TEXT("Takeoff Task: Sending takeoff command..."));
        bool result = false;
        client.takeoffAsync(Timeout, vehicle_name_std)->waitOnLastTask(&result);
        
        if (result)
        {
            UE_LOG(LogTemp, Log, TEXT("Takeoff Task: Takeoff successful."));
            bSuccess = true;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Takeoff Task: Takeoff command failed."));
            bSuccess = false;
        }
    }
    catch (const std::exception& e)
    {
        UE_LOG(LogTemp, Error, TEXT("Takeoff task failed with exception: %s"), ANSI_TO_TCHAR(e.what()));
        bSuccess = false;
    }

    bIsFinished = true;
    return 0;
}

void FTakeoffWorker::Stop()
{
}

bool FTakeoffWorker::IsFinished() const
{
    return bIsFinished;
}

bool FTakeoffWorker::WasSuccessful() const
{
    return bSuccess;
}
