#include "ZwQuerySystemInformationHooks.h"


extern "C"  NTSTATUS NtQuerySystemInformationHook_HideProcess(ULONG SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength)
{
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "HideProcess");    
    DbgBreakPoint();

    auto hookData = SSDTHookManagaer::getInstance().GetHookByName("NtQuerySystemInformation");
    NT_QUERY_SYSTEM_INFORMATION originalFunc = reinterpret_cast<NT_QUERY_SYSTEM_INFORMATION>(hookData.OriginalFunc);

    auto hookData2 = SSDTHookManagaer::getInstance().GetHookByAddres(reinterpret_cast<ULONG_PTR>(&NtQuerySystemInformationHook_HideProcess));
    NT_QUERY_SYSTEM_INFORMATION originalFunc2 = reinterpret_cast<NT_QUERY_SYSTEM_INFORMATION>(hookData2.OriginalFunc);
        
    return originalFunc(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength);
}
