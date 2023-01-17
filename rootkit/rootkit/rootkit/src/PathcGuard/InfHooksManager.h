#pragma once
#include "../stdafx.h"
#include "../KernelManager/KernelManager.h"
#include "../SSDTManager/SSDTManager.h"
#include "../Structs/Structs.h"

EXTERN_C
NTSYSCALLAPI
NTSTATUS
NTAPI
ZwTraceControl(
    _In_ ULONG FunctionCode,
    _In_reads_bytes_opt_(InBufferLen) PVOID InBuffer,
    _In_ ULONG InBufferLen,
    _Out_writes_bytes_opt_(OutBufferLen) PVOID OutBuffer,
    _In_ ULONG OutBufferLen,
    _Out_ PULONG ReturnLength
);

class InfHooksManager
{
    InfHooksManager() = default;

    InfHooksManager(const InfHooksManager&) = delete;    

    ULONG_PTR etwpDebuggerData; 

    ULONG_PTR etwpDebuggerDataSilo;

    ULONG_PTR wmiLoggerContext;

    ULONG_PTR halpPerformanceCounter;

    List<SSDTHook> infHook;

public:

    static InfHooksManager& getInstance()
    {
        static InfHooksManager instance;
        return instance;
    }

    BOOL InitializeInstanceData();

    BOOL AddHook(SSDTHook hook);

    SSDTHook FindInList(ULONG_PTR addr);

    SSDTHook FindInListByOriginAddr(ULONG_PTR addr);
};
