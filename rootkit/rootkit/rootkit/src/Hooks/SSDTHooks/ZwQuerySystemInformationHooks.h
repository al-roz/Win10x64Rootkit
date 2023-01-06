#pragma once 
#include "../../stdafx.h"
#include "../../SSDTManager/SSDTHooksManager.h"
#include "../../SplicingManager/SplicingManager.h"


#define SystemProcessesAndThreadsInformation 5
#define SystemModuleInformation 11

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