#pragma once
#include "SSDTManager.h"

class SSDTHookManagaer
{
private:    

    List<SSDTHook> SSDTHooks;

    SSDTHookManagaer() = default;

    SSDTHookManagaer(const SSDTHookManagaer&) = delete;

public:
    static SSDTHookManagaer& getInstance()
    {
        static SSDTHookManagaer instance;
        return instance;
    }

    BOOL SetHook(SSDTHook hook);

    SSDTHook GetHookByName(PCHAR funcName);

    SSDTHook GetHookByAddres(ULONG_PTR funcAddres);

    BOOL UnHook(PCHAR funcName);    

    BOOL OnUnload();
};
