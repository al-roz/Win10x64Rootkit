#include "SSDTHooksManager.h"

BOOL SSDTHookManagaer::SetHook(SSDTHook hook)
{
    
     hook.index = SSDTManager::getInstance().GetIndexSyscallFromNtdll(hook.funcName);
    if (hook.index == -1)
    {
        return FALSE;
    }

    hook.OriginalFunc = SSDTManager::getInstance().GetFuncAddres(hook.index);
    if (hook.OriginalFunc == NULL)
    {
        return FALSE;
    }

    DbgBreakPoint();

    auto ptrInKernelZeroMemmory = KernelManager::getInstance().GetZeroMemmoryInSections(KernelManager::getInstance().GetSectionByAddres(hook.OriginalFunc));

    HOOKOPCODES opcode;
    opcode.addr = hook.HookFunc;

    SuperCopyMemory((PVOID)ptrInKernelZeroMemmory, &opcode, sizeof(HOOKOPCODES));

    SSDTManager::getInstance().SetFuncInSSDT(hook.index, ptrInKernelZeroMemmory);
    DbgBreakPoint();
    this->SSDTHooks.AddElement(hook);

    return TRUE;
}

SSDTHook SSDTHookManagaer::GetHookByName(PCHAR funcName)
{
    auto findFunc = [&funcName](SSDTHook hook) {
        return _stricmp(hook.funcName, funcName) == 0;
    };

    return this->SSDTHooks.Find(findFunc);
}

SSDTHook SSDTHookManagaer::GetHookByAddres(ULONG_PTR funcAddres)
{
    auto findFunc = [&funcAddres](SSDTHook hook) {
        return hook.HookFunc == funcAddres;
    };

    return this->SSDTHooks.Find(findFunc);
}

BOOL SSDTHookManagaer::UnHook(PCHAR funcName)
{
	return 0;
}

BOOL SSDTHookManagaer::OnUnload()
{
    auto freeFunc = [](SSDTHook hook)
    {
        SSDTManager::getInstance().SetFuncInSSDT(hook.index, hook.OriginalFunc);
    };

    this->SSDTHooks.FreeList(freeFunc);

    return TRUE;
}
