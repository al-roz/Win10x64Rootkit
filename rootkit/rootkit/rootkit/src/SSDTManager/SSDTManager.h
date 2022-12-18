#pragma once
#include <ntddk.h>
#include <basetsd.h>
#include <windef.h>

#include "../Utils/Utils.h"
#include "../Utils/List.h"
#include "../KernelManager/KernelManager.h"
#include "../LoadedPe/LoadedPe.h"

struct SSDTStruct
{
    LONG* pServiceTable;
    PVOID pCounterTable;
#ifdef _WIN64
    ULONGLONG NumberOfServices;
#else
    ULONG NumberOfServices;
#endif
    PCHAR pArgumentTable;
};

class SSDTManager
{
private:
    SSDTStruct* ssdtShadow;
    SSDTStruct* ssdt;

    PDWORD KiServiceTable;

    LoadedPe ntdll;

    List<SSDTHook> SSDTHooks;

    SSDTManager() = default;
    
    SSDTManager(const SSDTManager&) = delete;        

public:
    static SSDTManager & getInstance()
    {
        static SSDTManager instance;
        return instance;
    }

    BOOL InitializeInstanceData();    

    DWORD GetIndexSyscallFromNtdll(PCHAR funcName);

    ULONG_PTR GetFuncAddres(DWORD index);

    template <typename func>
    BOOL SetHook(PCHAR funcName,func ptrFunc);

    BOOL UnHook(PCHAR funcName);

    void FreeData();

};

template<typename func>
BOOL SSDTManager::SetHook(PCHAR funcName, func ptrFunc)
{
    auto funcIndexFromSSDT = this->GetIndexSyscallFromNtdll(funcName);
    auto funcAddres = this->GetFuncAddres(funcIndexFromSSDT);



    return TRUE;
}
