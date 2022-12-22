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

    SSDTManager() = default;
    
    SSDTManager(const SSDTManager&) = delete;        

    DWORD FuncOffsetSSDT(ULONG_PTR func);

public:
    static SSDTManager & getInstance()
    {
        static SSDTManager instance;
        return instance;
    }

    BOOL InitializeInstanceData();    

    DWORD GetIndexSyscallFromNtdll(PCHAR funcName);

    ULONG_PTR GetFuncAddres(DWORD index);

    BOOL SetFuncInSSDT(DWORD index, ULONG_PTR funcAddres);

    void FreeData();

};