#include "RotateToYawWorker.h"
#include "AirBlueprintLib.h"

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

FRotateToYawWorker::FRotateToYawWorker(msr::airlib::MultirotorRpcLibClient* client, float yaw, float timeout_sec, const FString& vehicle_name)
    : Client(client)
    , Yaw(yaw)
    , Timeout(timeout_sec)
    , VehicleName(vehicle_name)
    , bIsFinished(false)
    , bSuccess(false)
{
    Thread = FRunnableThread::Create(this, TEXT("RotateToYawWorkerThread"), 0, TPri_BelowNormal);
}

FRotateToYawWorker::~FRotateToYawWorker()
{
    if (Thread)
    {
        Thread->Kill(true);
        delete Thread;
    }
}

bool FRotateToYawWorker::Init()
{
    return true;
}

uint32 FRotateToYawWorker::Run()
{
    try
    {
        std::string vehicle_name_std = TCHAR_TO_UTF8(*VehicleName);
        
        bool result = false;
        Client->rotateToYawAsync(Yaw, Timeout, 5, vehicle_name_std)->waitOnLastTask(&result);
        bSuccess = result;
    }
    catch (const std::exception& e)
    {
        UE_LOG(LogTemp, Error, TEXT("RotateToYaw task failed with exception: %s"), ANSI_TO_TCHAR(e.what()));
        bSuccess = false;
    }

    bIsFinished = true;
    return 0;
}

void FRotateToYawWorker::Stop()
{
}

bool FRotateToYawWorker::IsFinished() const
{
    return bIsFinished;
}

bool FRotateToYawWorker::WasSuccessful() const
{
    return bSuccess;
}
