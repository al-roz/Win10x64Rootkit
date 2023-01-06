#pragma once
#include "Structs\Structs.h"
#include "Utils\List.h"

struct RegistryKey
{
    PWCHAR registryName;
    ULONG registryLength;
    List<SubKey> subkeysList;
};

class RootkitHooksConfig
{    
    List<Process> hiddenProcessesList;
    List<RegistryKey> RegistryList;
public:

    

    static RootkitHooksConfig& getInstance()
    {
        static RootkitHooksConfig instance;
        return instance;
    }

    List<Process> getHiddenProcessList();

    List<RegistryKey> getRegistryList();
};
