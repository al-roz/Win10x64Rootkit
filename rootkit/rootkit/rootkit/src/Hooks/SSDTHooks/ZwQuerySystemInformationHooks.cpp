#include "ZwQuerySystemInformationHooks.h"

NTSTATUS HookNtQuerySystemInformation_HideProcess(ULONG SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength)
{
    return 0;
}
