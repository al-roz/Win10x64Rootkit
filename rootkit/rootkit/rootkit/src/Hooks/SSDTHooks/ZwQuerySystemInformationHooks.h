#pragma once
#include <ntddk.h>

#include <basetsd.h>
#include <windef.h>


typedef NTSTATUS(*NT_QUERY_SYSTEM_INFORMATION)(
    ULONG SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength);

NTSTATUS HookNtQuerySystemInformation_HideProcess(
    ULONG SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
);