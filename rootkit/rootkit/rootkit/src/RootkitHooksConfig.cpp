#include "RootkitHooksConfig.h"

List<ProcessName>& RootkitHooksConfig::GetHiddenProcessByNameList()
{
	return this->hiddenProcessesByNameList;
}

List<ProcessPID>& RootkitHooksConfig::GetHiddenProcessByPIDList()
{
	return this->hiddenProcessesByPIDList;
}

List<RegistryKey>& RootkitHooksConfig::GetRegistryList()
{
	return this->RegistryList;
}

List<INTERNAL_TCP_TABLE_ENTRY>& RootkitHooksConfig::GetTcpEntryList()
{
	return this->tcpEntryList;
}
