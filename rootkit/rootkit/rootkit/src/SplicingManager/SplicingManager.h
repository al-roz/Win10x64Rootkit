#pragma once
#include "../Structs/Structs.h"
#include "../Utils/List.h"
#include "../Utils/hde64.h"
#include "../Utils/Utils.h"

class SplicingManager
{
    List<SplicingHook> hooks;

public:

    static SplicingManager& getInstance()
    {
        static SplicingManager instance;
        return instance;
    }

    BOOL InitializeInstanceData();

    BOOL SetHook(SplicingHook hook);

    SplicingHook GetHookByAddres(ULONG_PTR adderes);

    BOOL UnHook(PCHAR funcName);

    BOOL OnUnload();    
};
