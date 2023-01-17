#pragma once
#include "Structs\Structs.h"
#include "Utils\List.h"

class RootkitHooksConfig
{    
    List<ProcessName> hiddenProcessesByNameList;
    List<ProcessPID> hiddenProcessesByPIDList;
    List<RegistryKey> RegistryList;
    List<INTERNAL_TCP_TABLE_ENTRY> tcpEntryList;

    RootkitHooksConfig() = default;

    RootkitHooksConfig(const RootkitHooksConfig&) = delete;
    
public:    

    static RootkitHooksConfig& getInstance()
    {
        static RootkitHooksConfig instance;
        return instance;
    }

    List<ProcessName>& GetHiddenProcessByNameList();
    
    List<ProcessPID>& GetHiddenProcessByPIDList();

    List<RegistryKey>& GetRegistryList();

    List<INTERNAL_TCP_TABLE_ENTRY>& GetTcpEntryList();
};
