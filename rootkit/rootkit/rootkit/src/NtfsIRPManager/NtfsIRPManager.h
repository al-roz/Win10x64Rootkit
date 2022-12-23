#pragma once
#include <ntddk.h>
#include <basetsd.h>
#include <windef.h>

#include "../Utils/List.h"
#include "../Structs/Structs.h"

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

    BOOL SetHook(IRPHook hook);

    IRPHook FindHook(IRP_HANDLER hookHandler);
};
