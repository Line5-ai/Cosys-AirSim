#include "MoveToZWorker.h"
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
#include "NedTransform.h"

FMoveToZWorker::FMoveToZWorker(msr::airlib::MultirotorRpcLibClient* client, float z, float velocity, float timeout_sec, const FString& vehicle_name, float world_to_meters)
    : Client(client)
    , Z(z)
    , Velocity(velocity)
    , Timeout(timeout_sec)
    , VehicleName(vehicle_name)
    , WorldToMetersScale(world_to_meters)
    , bIsFinished(false)
    , bSuccess(false)
{
    Thread = FRunnableThread::Create(this, TEXT("MoveToZWorkerThread"), 0, TPri_BelowNormal);
}

FMoveToZWorker::~FMoveToZWorker()
{
    if (Thread)
    {
        Thread->Kill(true);
        delete Thread;
    }
}

bool FMoveToZWorker::Init()
{
    return true;
}

uint32 FMoveToZWorker::Run()
{
    UE_LOG(LogTemp, Log, TEXT("MoveToZ Task: Sending command..."));

    try
    {
        // Convert from Unreal cm to AirSim meters and Z-up to Z-down
        float ned_z = -Z / WorldToMetersScale;
        std::string vehicle_name_std = TCHAR_TO_UTF8(*VehicleName);
        Client->moveToZAsync(ned_z, Velocity, Timeout, msr::airlib::YawMode(), -1, 1, vehicle_name_std)->waitOnLastTask();
        bSuccess = true;
    }
    catch (const rpc::rpc_error& e)
    {
        UE_LOG(LogTemp, Error, TEXT("MoveToZ task failed with exception: %s"), ANSI_TO_TCHAR(e.what()));
        bSuccess = false;
    }

    bIsFinished = true;
    return 0;
}

void FMoveToZWorker::Stop()
{
}

bool FMoveToZWorker::IsFinished() const
{
    return bIsFinished;
}

bool FMoveToZWorker::WasSuccessful() const
{
    return bSuccess;
}
