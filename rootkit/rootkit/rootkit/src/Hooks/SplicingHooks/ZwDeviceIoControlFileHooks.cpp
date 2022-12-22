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

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ZwDeviceIoControlFileHooks\n");

	auto originalFunc = reinterpret_cast<ZW_DEVICE_IO_CONTROL_FILE>(hookData.wrapper);

	DbgBreakPoint();

	return originalFunc(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, IoControlCode, InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength);
}
