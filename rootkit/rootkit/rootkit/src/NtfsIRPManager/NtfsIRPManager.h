#pragma once
#include "../stdafx.h"
#include "../Structs/Structs.h"
#include "../Utils/List.h"


class NtfsIRPManager
{
private:

    NtfsIRPManager() = default;
    NtfsIRPManager(const NtfsIRPManager&) = delete;

    PDEVICE_OBJECT DeviceObject;
    PFILE_OBJECT FileObject;

    List<IRPHook> hooks;

public:

    static NtfsIRPManager& getInstance()
    {
        static NtfsIRPManager instance;
        return instance;
    }

    BOOLEAN InitializeInstanceData();

    PDEVICE_OBJECT GetDeviceObject();

    BOOLEAN SetHook(IRPHook hook);

    IRPHook FindHook(IRP_HANDLER hookHandler);

    BOOLEAN OnUnload();
};
