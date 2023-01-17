#include "InfHooksManager.h"

extern "C" {
	ULONG_PTR halCounterQueryRoutine{};
	ULONG_PTR circularKernelContextLogger{};
	ULONG_PTR targetSystemCallFunction{};
	ULONG_PTR systemCallHookFunction{};
	void testHook(ULONG64* pStack);
	void checkLogger();
}

void testHook(ULONG64* pStack)
{		

	if (ExGetPreviousMode() == KernelMode)
	{
		return;
	}	

	for (size_t i = 0; i < 10; i++)
	{
		if (pStack[i] == circularKernelContextLogger)
		{			
			ULONG_PTR currentThread = reinterpret_cast<ULONG_PTR>(KeGetCurrentThread());
			UINT32 syscallNumber = *reinterpret_cast<UINT32*>(currentThread + 0x80);
			if (!syscallNumber)
				return;

			auto systemRoutine = SSDTManager::getInstance().GetFuncAddres(syscallNumber);

			ULONG_PTR stackLowLimit, stackHighLimit;
			IoGetStackLimits(&stackLowLimit, &stackHighLimit);
			
			auto hook = InfHooksManager::getInstance().FindInListByOriginAddr(systemRoutine);
			if (!hook.HookFunc)
				return;			

			for (auto stack = (ULONG_PTR)pStack; stack < stackHighLimit - sizeof(ULONG_PTR); stack += sizeof(ULONG_PTR))
			{
				if (*reinterpret_cast<UINT64*>(stack) == systemRoutine)
				{
					*reinterpret_cast<UINT64*>(stack) = hook.HookFunc;
					
					return;
				}
			}			
			return;
		}
	}

	return;
}

ULONG_PTR getCKCL()
{
	SIZE_T kernelDataSectionsSize = 0;
	auto dataSection = KernelManager::getInstance().GetSectionByName(".data", &kernelDataSectionsSize);
	if (!dataSection)
	{
		return {};
	}

	BYTE patternCKCL[] = { 0x2C,0x08,0x04,0x38,0x0C };
	SIZE_T patternLenght = sizeof(patternCKCL);

	auto data = findBySignature((PBYTE)dataSection, kernelDataSectionsSize, patternCKCL, patternLenght);
	if (data)
	{
		return data - 2;
	}
	return {};
}

NTSTATUS  ModifyTraceSettings(ETWTRACECONTROLCODE functionCode, UINT32 enableFlags)
{
	PCKCL_TRACE_PROPERTIES Property = (PCKCL_TRACE_PROPERTIES)ExAllocatePool(NonPagedPool, PAGE_SIZE);
	if (!Property)
	{
		return STATUS_MEMORY_NOT_ALLOCATED;
	}

	memset(Property, 0, PAGE_SIZE);	

	Property->Wnode.BufferSize = PAGE_SIZE;
	Property->Wnode.Guid = { 0x54DEA73A, 0xED1F, 0x42A4, {0xAF, 0x71, 0x3E, 0x63, 0xD0, 0x56, 0xF1, 0x74} };
	Property->Wnode.ClientContext = 0x1;
	Property->Wnode.Flags = 0x20000;
	Property->BufferSize = sizeof(UINT32);
	Property->MinimumBuffers = 2;
	Property->MaximumBuffers = 2;
	Property->LogFileMode = 0x400;
	Property->EnableFlags = enableFlags;
	Property->ProviderName = RTL_CONSTANT_STRING(L"Circular Kernel Context Logger");	

	UINT32 returnSize{};

	return ZwTraceControl(functionCode, Property, PAGE_SIZE, Property, PAGE_SIZE, reinterpret_cast<PULONG>(&returnSize));

}

BOOL InfHooksManager::InitializeInstanceData()
{	
	this->etwpDebuggerData = getCKCL();
	
	if (!this->etwpDebuggerData)
	{
		return FALSE;
	}

	this->etwpDebuggerDataSilo =  *reinterpret_cast<ULONG_PTR*>(this->etwpDebuggerData + 0x10);
	if (!this->etwpDebuggerDataSilo)
	{
		return FALSE;
	}

	this->wmiLoggerContext = reinterpret_cast<ULONG_PTR*>(this->etwpDebuggerDataSilo)[2];
	if (!this->wmiLoggerContext)
	{
		return FALSE;
	}

	circularKernelContextLogger = this->wmiLoggerContext;

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%p\n", circularKernelContextLogger);

	auto keQueryPerformanceCounter = KernelManager::getInstance().FindFunctionFromExportByName("KeQueryPerformanceCounter").functionAddres;
	BYTE pattern[] = { 0x48,0x8b,0x3d}; //488b3d
	SIZE_T patternLenght = sizeof(pattern);

	auto data = findBySignature(reinterpret_cast<PBYTE>(keQueryPerformanceCounter), 0x100, pattern, patternLenght);
	if (!data)
	{
		return false;
	}
	auto offset = *reinterpret_cast<LONG*>(data + patternLenght);
	
	this->halpPerformanceCounter = (data + offset + 7);	

	if (!NT_SUCCESS(ModifyTraceSettings(EtwUpdateLoggerCode, EVENT_TRACE_FLAG_SYSTEMCALL)))
		if (!NT_SUCCESS(ModifyTraceSettings(EtwStartLoggerCode, EVENT_TRACE_FLAG_SYSTEMCALL)))
			return false;
		else
		{
			if (!NT_SUCCESS(ModifyTraceSettings(EtwUpdateLoggerCode, EVENT_TRACE_FLAG_SYSTEMCALL)))
			{
				return false;
			}
		}	

	*reinterpret_cast<UINT64*>(this->wmiLoggerContext + 0x28) = 1;
	
	halCounterQueryRoutine = *reinterpret_cast<ULONG_PTR*>(*reinterpret_cast<ULONG_PTR*>(this->halpPerformanceCounter) + 0x70);
	*reinterpret_cast<ULONG_PTR*>(*reinterpret_cast<ULONG_PTR*>(this->halpPerformanceCounter) + 0x70) = (ULONG_PTR)&checkLogger;
	
	return true;
}

BOOL InfHooksManager::AddHook(SSDTHook hook)
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

	this->infHook.AddElement(hook);

	return TRUE;
}

SSDTHook InfHooksManager::FindInList(ULONG_PTR funcAddres)
{
	auto findFunc = [&funcAddres](SSDTHook hook) {
		return hook.HookFunc == funcAddres;
	};

	return this->infHook.Find(findFunc);
}

SSDTHook InfHooksManager::FindInListByOriginAddr(ULONG_PTR funcAddres)
{
	auto findFunc = [&funcAddres](SSDTHook hook) {
		return hook.OriginalFunc == funcAddres;
	};

	return this->infHook.Find(findFunc);
}
