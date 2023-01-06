
#include "ZwDeviceIoControlFileHooks.h"

NTSTATUS ZwDeviceIoControlFileHook_AddElement(
	IN HANDLE FileHandle,
	IN HANDLE Event,
	IN PIO_APC_ROUTINE ApcRoutine,
	IN PVOID ApcContext,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN ULONG IoControlCode,
	IN PVOID InputBuffer,
	IN ULONG InputBufferLength,
	OUT PVOID OutputBuffer,
	IN ULONG OutputBufferLength)
{

	auto hookData = SplicingManager::getInstance().GetHookByAddres((ULONG_PTR)&ZwDeviceIoControlFileHook_AddElement);	

	auto originalFunc = reinterpret_cast<ZW_DEVICE_IO_CONTROL_FILE>(hookData.wrapper);	
	
	NTSTATUS status = originalFunc(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, IoControlCode, InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength);

	if (!NT_SUCCESS(status))
		return status;	

	
	if (IoControlCode != 0x12001B)
	{
		return status;
	}

	if (OutputBuffer == NULL)
	{
		return status;
	}

	PNSI_PARAM ptrNsiParam = reinterpret_cast<PNSI_PARAM>(OutputBuffer);
	
	if (ptrNsiParam->typeInfo != 0x38)
	{
		return status;
	}
	

	PINTERNAL_TCP_TABLE_ENTRY fakeBuffer = 0;	
	PNSI_PARAM fakeBufferTest = 0;	

	SIZE_T fakeBufferSize = (ptrNsiParam->tcpConnectionCount) * sizeof(INTERNAL_TCP_TABLE_ENTRY);	
	SIZE_T fakeBufferSizeTest = sizeof(NSI_PARAM);

	PNSI_PARAM t = reinterpret_cast<PNSI_PARAM>(ExAllocatePool(NonPagedPool, fakeBufferSizeTest));

	if (!t)
	{
		return status;
	}
	
	return status;
}
