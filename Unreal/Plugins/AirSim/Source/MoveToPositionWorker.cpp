#include "MoveToPositionWorker.h"
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

FMoveToPositionWorker::FMoveToPositionWorker(msr::airlib::MultirotorRpcLibClient* client, const FVector& position, float velocity, float timeout_sec, const FString& vehicle_name, float world_to_meters)
    : Client(client)
    , Position(position)
    , Velocity(velocity)
    , Timeout(timeout_sec)
    , VehicleName(vehicle_name)
    , WorldToMetersScale(world_to_meters)
    , bIsFinished(false)
    , bSuccess(false)
{
    Thread = FRunnableThread::Create(this, TEXT("MoveToPositionWorkerThread"), 0, TPri_BelowNormal);
}

FMoveToPositionWorker::~FMoveToPositionWorker()
{
    if (Thread)
    {
        Thread->Kill(true);
        delete Thread;
    }
}

bool FMoveToPositionWorker::Init()
{
    return true;
}

uint32 FMoveToPositionWorker::Run()
{
    UE_LOG(LogTemp, Log, TEXT("MoveToPosition Task: Sending command..."));

    try
    {
        // Convert from Unreal cm to AirSim meters and Unreal coords to NED
        float scale = 1.0f / WorldToMetersScale;
        msr::airlib::Vector3r ned_position(Position.X * scale, Position.Y * scale, -Position.Z * scale);

        std::string vehicle_name_std = TCHAR_TO_UTF8(*VehicleName);
        Client->moveToPositionAsync(ned_position.x(), ned_position.y(), ned_position.z(), Velocity, Timeout, 
            msr::airlib::DrivetrainType::MaxDegreeOfFreedom, msr::airlib::YawMode(), -1, 1, vehicle_name_std)->waitOnLastTask();
        bSuccess = true;
    }
    catch (const rpc::rpc_error& e)
    {
        UE_LOG(LogTemp, Error, TEXT("MoveToPosition task failed with exception: %s"), ANSI_TO_TCHAR(e.what()));
        bSuccess = false;
    }

    bIsFinished = true;
    return 0;
}

void FMoveToPositionWorker::Stop()
{
}

bool FMoveToPositionWorker::IsFinished() const
{
    return bIsFinished;
}

bool FMoveToPositionWorker::WasSuccessful() const
{
    return bSuccess;
}
