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

	PBYTE wrapper = (PBYTE)ExAllocatePool(NonPagedPool, offset + sizeof(SPLICINGOPCODES));

	if (!wrapper)
	{
		return FALSE;
	}

	RtlCopyMemory(wrapper,(PBYTE)hook.OriginalFunc, offset);

	KIRQL irql = WPOFFx64();

	*((PWORD)hook.OriginalFunc) = 0xb848;
	*(PVOID*)(hook.OriginalFunc + 2) = (PBYTE)hook.HookFunc;
	*((PWORD)(hook.OriginalFunc + 10)) = 0xe0ff;

	*((PWORD)(wrapper + offset)) = 0xb848;
	*(PVOID*)(wrapper + offset + 2) = (PBYTE)hook.OriginalFunc + offset;
	*((PWORD)(wrapper + offset + 10)) = 0xe0ff;
	
	WPONx64(irql);

	hook.wrapper = (ULONG_PTR)wrapper;

	this->hooks.AddElement(hook);

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
	return 0;
}
