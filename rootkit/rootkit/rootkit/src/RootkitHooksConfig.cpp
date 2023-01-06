#include "RootkitHooksConfig.h"

List<Process> RootkitHooksConfig::getHiddenProcessList()
{
	return this->hiddenProcessesList;
}

List<RegistryKey> RootkitHooksConfig::getRegistryList()
{
	return this->RegistryList;
}
