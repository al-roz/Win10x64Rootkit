#include "SplicingManager.h"

BOOL SplicingManager::InitializeInstanceData()
{
	return TRUE;
}

BOOL SplicingManager::SetHook(SplicingHook hook)
{

	hde64s disasState;
	ULONG offset = 0;

	if (!hook.OriginalFunc)
	{
		return FALSE;
	}

	if (!hook.HookFunc)
	{
		return FALSE;
	}	

	while (offset < sizeof(SPLICINGOPCODES))
	{
		hde64_disasm((PBYTE)hook.OriginalFunc + offset, &disasState);
		offset += disasState.len;
	}

	PBYTE wrapper = (PBYTE)ExAllocatePool(NonPagedPool, offset + sizeof(JAMPTAG));

	if (!wrapper)
	{
		return FALSE;
	}

	RtlCopyMemory(wrapper, (PBYTE)hook.OriginalFunc, offset);
	//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%p", wrapper);	

	JAMPTAG opcode;
	opcode.addr = hook.OriginalFunc + offset;

	SuperCopyMemory((PVOID)(wrapper + offset), &opcode, sizeof(JAMPTAG));

	/*unsigned short int jmp = 0x25ff;
	unsigned int tag = 0x00000000;
	ULONG_PTR addr;*/

	//*((PWORD)(wrapper + offset)) = 0x25ff;
	//*(PVOID*)(wrapper + offset + 2) = 0x00000000;
	//*((PWORD)(wrapper + offset + 6)) = hook.OriginalFunc + offset;
	
	hook.wrapper = (ULONG_PTR)wrapper;

	this->hooks.AddElement(hook);

	KIRQL irql = WPOFFx64();

	*((PWORD)hook.OriginalFunc) = 0xb848;
	*(PVOID*)(hook.OriginalFunc + 2) = (PBYTE)hook.HookFunc;
	*((PWORD)(hook.OriginalFunc + 10)) = 0xe0ff;
	
	WPONx64(irql);			

	DbgBreakPoint();

	return TRUE;
}

SplicingHook SplicingManager::GetHookByAddres(ULONG_PTR adderess)
{
	auto findFunc = [&adderess](SplicingHook hook) {
		return hook.HookFunc == adderess;
	};

	return this->hooks.Find(findFunc);
}

BOOL SplicingManager::UnHook(PCHAR funcName)
{
	return 0;
}

BOOL SplicingManager::OnUnload()
{
	auto freeFunc = [](SplicingHook hook)
	{		
		SuperCopyMemory((PVOID)hook.OriginalFunc, (PVOID)hook.wrapper, sizeof(SPLICINGOPCODES));
		ExFreePool((PVOID)hook.wrapper);
	};
	
	this->hooks.FreeList(freeFunc);

	return TRUE;
}
