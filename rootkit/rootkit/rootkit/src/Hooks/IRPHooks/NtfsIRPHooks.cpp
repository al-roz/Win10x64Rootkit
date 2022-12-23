#include "NtfsIRPHooks.h"

NTSTATUS HookDirectoryControl(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp)
{
	auto hookData = NtfsIRPManager::getInstance().FindHook(HookDirectoryControl);

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "IRP hook\n");

	return hookData.OriginalFunc(pDeviceObject, pIrp);
}
