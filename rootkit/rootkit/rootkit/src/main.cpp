#include <ntddk.h>
#include <basetsd.h>
#include <windef.h>
#include <dontuse.h>
#include <suppress.h>

#include "SSDTManager\SSDTManager.h"
#include "KernelManager\KernelManager.h"

extern "C" DRIVER_INITIALIZE DriverEntry;
extern "C" DRIVER_UNLOAD DriverUnload;

extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT driverObject, IN PUNICODE_STRING registryPath)
{
    NTSTATUS retStatus;

    UNREFERENCED_PARAMETER(registryPath);
    
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"rootkit start\n");

    DbgBreakPoint();

    if (!KernelManager::getInstance().InitializeInstanceData())
    {
        retStatus = STATUS_FAILED_DRIVER_ENTRY;
        return retStatus;
    }    

    if (!SSDTManager::getInstance().InitializeInstanceData())
    {
        retStatus = STATUS_FAILED_DRIVER_ENTRY;
        return retStatus;
    }

    driverObject->DriverUnload = DriverUnload;
    retStatus = STATUS_SUCCESS;
    return retStatus;
}

VOID DriverUnload(IN PDRIVER_OBJECT DriverObject)
{
    UNREFERENCED_PARAMETER(DriverObject);

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Driver unload\n");
    return;
}
