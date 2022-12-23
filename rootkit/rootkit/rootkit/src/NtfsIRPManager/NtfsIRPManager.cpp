#include "NtfsIRPManager.h"

NTSTATUS HookDirectoryControl(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp);

BOOLEAN NtfsIRPManager::InitializeInstanceData()
{
	UNICODE_STRING deviceName;
	RtlInitUnicodeString(&deviceName, L"\\NTFS");
	DbgBreakPoint();
	NTSTATUS status = IoGetDeviceObjectPointer(&deviceName,
		FILE_READ_ATTRIBUTES,
		&this->FileObject,
		&this->DeviceObject);

	return NT_SUCCESS(status);
}

BOOL NtfsIRPManager::SetHook(IRPHook hook)
{
	DbgBreakPoint();
	if (hook.IRP == -1)
	{
		return FALSE;
	}

	if (hook.HookFunc == NULL)
	{
		return FALSE;
	}

	hook.OriginalFunc = this->DeviceObject->DriverObject->MajorFunction[hook.IRP];
	this->DeviceObject->DriverObject->MajorFunction[hook.IRP] = hook.HookFunc;

	this->hooks.AddElement(hook);

	return TRUE;
}

IRPHook NtfsIRPManager::FindHook(IRP_HANDLER hookHandler)
{
	auto findFunc = [&hookHandler](IRPHook hook) {
		return hook.HookFunc == hookHandler;
	};

	return this->hooks.Find(findFunc);
}
